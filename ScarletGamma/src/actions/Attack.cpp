#include "Attack.hpp"
#include "Game.hpp"
#include "Constants.hpp"
#include "gamerules/CombatRules.hpp"
#include "network/Messenger.hpp"
#include "network/ActionMessages.hpp"
#include "states/StateMachine.hpp"
#include "states/PromptState.hpp"
#include "utils/ValueInterpreter.hpp"
#include "ActionPool.hpp"
#include "network/ChatMessages.hpp"
#include "core/World.hpp"
#include "gamerules/CombatRules.hpp"
#include "utils/Exception.hpp"

using namespace Actions;
using namespace GameRules;

Attack::Attack() : Action(STR_ACT_ATTACK, ActionType::STANDARD_ACTION, 50, Game::MC_ATTACK)
{
    // Set requirements
    m_targetRequirements.push_back(std::pair<std::string, bool>(STR_PROP_HEALTH, true));
    m_targetRequirements.push_back(std::pair<std::string, bool>(STR_PROP_ARMORCLASS, true));
    m_finished = false;
}

Action* Attack::Clone(Core::ObjectID _executor, Core::ObjectID _target)
{
    Attack* result = new Attack();

    // Set all required values
    result->m_id = m_id;
    result->m_priority = m_priority;
    result->m_cursor = m_cursor;
    result->m_executor = _executor;
    result->m_target = _target;

    return dynamic_cast<Action*>(result);
}

void Attack::Execute()
{
    // Tell the server that we are starting an attack
    if (Network::Messenger::IsServer() == false)
        Network::MsgActionBegin(this->m_id, m_target).Send();

    // Open prompt for hit roll value
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(
        g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
    prompt->SetText("Angriffswurf eingeben:");
    prompt->AddPopCallback(std::bind(&Attack::AttackRollPromptFinished, this, std::placeholders::_1));
}

void Attack::HandleActionInfo(uint8_t _messageType, const std::string& _message, uint8_t _sender)
{
    // I am the DM. This function handles player's requests and information.
    assert(Network::Messenger::IsServer());
    m_sender = _sender;

    switch (static_cast<ActionMsgType>(_messageType))
    {
    case ActionMsgType::PL_ATTACK_ROLL_INFO:
        AttackRollInfoReceived(_message);
        break;
    case ActionMsgType::PL_HIT_ROLL_INFO:
        HitRollInfoReceived(_message);
        break;
    default:
        assert(false);
    }
}

void Attack::HandleActionInfoResponse(uint8_t _messageType, const std::string& _message)
{
    // I am a player. This function handles DM's responses.
    assert(Network::Messenger::IsServer() == false);

    switch (static_cast<ActionMsgType>(_messageType))
    {
     case ActionMsgType::DM_ATTACK_ROLL_MISS:
        AttackRollMissed();
        break;
    case ActionMsgType::DM_ATTACK_ROLL_HIT:
        AttackRollHit();
        break;
    case ActionMsgType::DM_ATTACK_ROLL_CRIT:
        // TODO: implement
        assert(false && "DM_ATTACK_ROLL_CRIT not implemented yet!");
        break;
    }
}

void Attack::AttackRollPromptFinished(States::GameState* gs)
{
    States::PromptState* ps = dynamic_cast<States::PromptState*>(gs);
    assert(ps && "Expected a PromptState on the top of the stack, wtf?");

    // Prompt is finished, what was the result?
    const std::string& result = ps->GetResult();
    if (result.empty())
    {
        // Tell the server that player canceled
        if (Network::Messenger::IsServer() == false)
            Network::MsgActionEnd(this->m_id).Send();

        // End the local action
        m_finished = true;
    }
    else
    {
        if (Network::Messenger::IsServer() == false)
        {
            // Tell the server about the result
            Network::MsgActionInfo(this->m_id, static_cast<uint8_t>(ActionMsgType::PL_ATTACK_ROLL_INFO), result).Send();
        }
        else
        {
            // Tell myself about the result
            AttackRollInfoLocal(result);
        }
    }
}

bool Attack::EvaluateAttackRoll(int _roll)
{
    // TODO implement properly :)

    try
    {
        return (_roll >= CombatRules::GetArmorClass(m_target));
    }
    catch (Exception::NoSuchProperty)
    {
        std::cerr << "Tried to attack an object without AC: " << std::to_string(m_executor)
            << " -> " << std::to_string(m_target) << '\n';
    }

    return false;
}

void Attack::AttackRollInfoReceived(const std::string& _message)
{
    assert(Network::Messenger::IsServer());

    // TODO: how to find out about natural 20?

    // Save attack roll for later
    m_attackRoll = _message;

    // Evaluate it
    int result = Utils::EvaluateFormula(_message, Game::RANDOM,
        g_Game->GetWorld()->GetObject(m_executor));

    // Push prompt
    PushAttackRollDMPrompt(result, &Attack::AttackRollDMPromptFinished);
}

void Attack::AttackRollInfoLocal(const std::string& _message)
{
    assert(Network::Messenger::IsServer());

    // TODO: how to find out about natural 20?

    // Save attack roll for later
    m_attackRoll = _message;

    // Evaluate it
    int result = Utils::EvaluateFormula(m_attackRoll, Game::RANDOM,
        g_Game->GetWorld()->GetObject(m_executor));

    // Push prompt
    PushAttackRollDMPrompt(result, &Attack::AttackRollDMPromptFinishedLocal);
}

void Attack::PushAttackRollDMPrompt(int _result, void (Attack::* _callback)(States::GameState*))
{
    // Open prompt for hit roll value
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(
        g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));

    std::string resultStr = std::to_string(_result);
    std::stringstream message;
    message << "Angriffswurf von " << g_Game->GetWorld()->GetObject(m_executor)->GetName()
            << " auf " << g_Game->GetWorld()->GetObject(m_target)->GetName() << '\n';
    message << m_attackRoll << " = " << resultStr << ", ";
    if (EvaluateAttackRoll(_result))
        message << "getroffen.\n";
    else
        message << "nicht getroffen.\n";
    message << "Der Wert kann angepasst werden. Das Ziel hat RK "
        << CombatRules::GetArmorClass(m_target) << " und "
        << CombatRules::GetHitPoints(m_target) << "TP.\n";

    prompt->SetText(message.str());
    prompt->SetDefaultValue(resultStr);
    prompt->AddPopCallback(std::bind(_callback, this, std::placeholders::_1));
}

void Attack::AttackRollDMPromptFinished(States::GameState* _gs)
{
    States::PromptState* ps = dynamic_cast<States::PromptState*>(_gs);
    assert(ps && "Expected a PromptState on the top of the stack, wtf?");

    int result = atoi( ps->GetResult().c_str() );

    if (EvaluateAttackRoll(result))
    {
        BroadcastHitMessage(m_attackRoll, result);
        SendAttackRollHit();
    }
    else
    {
        BroadcastMissMessage(m_attackRoll, result);
        SendAttackRollMissed();
    }
}

void Attack::AttackRollDMPromptFinishedLocal(States::GameState* _gs)
{
    States::PromptState* ps = dynamic_cast<States::PromptState*>(_gs);
    assert(ps && "Expected a PromptState on the top of the stack, wtf?");

    int result = atoi( ps->GetResult().c_str() );

    if (EvaluateAttackRoll(result))
    {
        BroadcastHitMessage(m_attackRoll, result);
        AttackRollHit();
    }
    else
    {
        BroadcastMissMessage(m_attackRoll, result);
        AttackRollMissed();
    }
}

void Attack::SendAttackRollHit()
{
    // Tell client that he hit
    Network::MsgActionInfo(this->m_id, static_cast<uint8_t>(ActionMsgType::DM_ATTACK_ROLL_HIT),
        STR_EMPTY).Send(m_sender);
}

void Attack::SendAttackRollMissed()
{
    // Tell client that he missed and end client action
    Network::MsgActionInfo(this->m_id, static_cast<uint8_t>(ActionMsgType::DM_ATTACK_ROLL_MISS),
        STR_EMPTY).Send(m_sender);
    Actions::ActionPool::Instance().EndClientAction(m_sender);
    // ACTION IS DONE
}

void Attack::AttackRollHit()
{
    // Prompt for damage
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
    prompt->SetText("Schadenswurf eingeben:");
    prompt->AddPopCallback(std::bind(&Attack::HitRollPromptFinished, this, std::placeholders::_1));
}

void Attack::AttackRollMissed()
{
    // End local action
    m_finished = true;
}

void Attack::HitRollPromptFinished(States::GameState* gs)
{
    States::PromptState* ps = dynamic_cast<States::PromptState*>(gs);
    assert(ps && "Expected a PromptState on the top of the stack, wtf?");

    // Prompt is finished, what was the result?
    const std::string& result = ps->GetResult();
    if (result.empty())
    {
        // Tell the server that player canceled
        if (Network::Messenger::IsServer() == false)
            Network::MsgActionEnd(this->m_id).Send();

        // End the local action
        m_finished = true;
    }
    else
    {
        if (Network::Messenger::IsServer() == false)
        {
            // Tell the server that about the result
            Network::MsgActionInfo(this->m_id, static_cast<uint8_t>(ActionMsgType::PL_HIT_ROLL_INFO), result).Send();
        }
        else
        {
            // Tell myself about the result
            HitRollInfoLocal(result);
        }
    }
}

void Attack::HitRollInfoReceived(const std::string& _message)
{
    assert(Network::Messenger::IsServer());

    // Save hit roll for later
    m_hitRoll = _message;

    // Evaluate it
    int result = Utils::EvaluateFormula(m_hitRoll, Game::RANDOM,
        g_Game->GetWorld()->GetObject(m_executor));

    // Push prompt
    PushHitRollDMPrompt(result, &Attack::HitRollDMPromptFinished);
}

void Attack::HitRollInfoLocal(const std::string& _message)
{
    assert(Network::Messenger::IsServer());

    // Save hit roll for later
    m_hitRoll = _message;

    // Evaluate it
    int result = Utils::EvaluateFormula(m_hitRoll, Game::RANDOM,
        g_Game->GetWorld()->GetObject(m_executor));

    // Push prompt
    PushHitRollDMPrompt(result, &Attack::HitRollDMPromptFinishedLocal);
}

void Attack::PushHitRollDMPrompt(int _result, void (Attack::* _callback)(States::GameState*))
{
    // Open prompt for hit roll value
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(
        g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));

    std::string resultStr = std::to_string(_result);
    std::stringstream message;
    message << "Trefferwurf von " << g_Game->GetWorld()->GetObject(m_executor)->GetName()
            << " auf " << g_Game->GetWorld()->GetObject(m_target)->GetName() << '\n';
    message << m_hitRoll << " = " << resultStr << ", ";
    message << "Der Wert kann angepasst werden. Das Ziel hat "
        << CombatRules::GetHitPoints(m_target) << "TP.\n";

    prompt->SetText(message.str());
    prompt->SetDefaultValue(resultStr);
    prompt->AddPopCallback(std::bind(_callback, this, std::placeholders::_1));
}

void Attack::HitRollDMPromptFinished(States::GameState* _gs)
{
    States::PromptState* ps = dynamic_cast<States::PromptState*>(_gs);
    assert(ps && "Expected a PromptState on the top of the stack, wtf?");

    // Apply damage
    int result = atoi( ps->GetResult().c_str() );
    CombatRules::ApplyHitDamage(m_target, result);
    BroadcastDamageMessage(m_hitRoll, result);

    // End client action
    ActionPool::Instance().EndClientAction(m_sender);
    Network::MsgActionEnd(m_id).Send(m_sender);
}

void Attack::HitRollDMPromptFinishedLocal(States::GameState* _gs)
{
    States::PromptState* ps = dynamic_cast<States::PromptState*>(_gs);
    assert(ps && "Expected a PromptState on the top of the stack, wtf?");

    // Apply damage
    int result = atoi( ps->GetResult().c_str() );
    CombatRules::ApplyHitDamage(m_target, result);
    BroadcastDamageMessage(m_hitRoll, result);

    // End local action and we're done
    m_finished = true;
}


////////////////////////////////////////////////////////////////////////////////
// BROADCAST MESSAGES

void Attack::BroadcastMissMessage(const std::string& _dice, int _result)
{
    // Only the server may broadcast
    assert(Network::Messenger::IsServer());

    // Broadcast miss message
    std::stringstream stream;
    stream << "Angriff von "
        << g_Game->GetWorld()->GetObject(m_executor)->GetName() << " auf "
        << g_Game->GetWorld()->GetObject(m_target)->GetName() << ": "
        << _dice << " = " << std::to_string(_result) << " - nicht getroffen";
    Network::ChatMsg message(stream.str(), sf::Color::White);
    message.Send();
}

void Attack::BroadcastHitMessage(const std::string& _dice, int _result)
{
    // Only the server may broadcast
    assert(Network::Messenger::IsServer());

    // Broadcast hit message
    std::stringstream stream;
    stream << "Angriff von "
        << g_Game->GetWorld()->GetObject(m_executor)->GetName() << " auf "
        << g_Game->GetWorld()->GetObject(m_target)->GetName() << ": "
        << _dice << " = " << std::to_string(_result) << " - getroffen!";
    Network::ChatMsg message(stream.str(), sf::Color::White);
    message.Send();
}

void Attack::BroadcastDamageMessage(const std::string& _dice, int _result)
{
    // Only the server may broadcast
    assert(Network::Messenger::IsServer());

    // Broadcast hit message
    std::stringstream stream;
    stream << "Trefferwurf " << _dice << " = " << std::to_string(_result) << " Schaden";
    Network::ChatMsg message(stream.str(), sf::Color::White);
    message.Send();
}
