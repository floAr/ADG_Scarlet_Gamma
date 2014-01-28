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

using namespace Actions;

Attack::Attack() : Action(STR_ACT_ATTACK)
{
    // Set requirements
    m_requirements.push_back(GameRules::CombatRules::PROP_HITPOINTS);
    m_requirements.push_back(GameRules::CombatRules::PROP_ARMORCLASS);
}

Action* Attack::Clone(Core::ObjectID _executor, Core::ObjectID _target)
{
    Attack* result = new Attack();

    result->m_executor = _executor; // set executor
    result->m_target = _target; // set target
    result->m_id = m_id; // copy ID

    return dynamic_cast<Action*>(result);
}

void Attack::Execute()
{
    // Tell the server that we are starting an attack
    if (Network::Messenger::IsServer() == false)
        Network::MsgActionBegin(this->GetID(), m_target).Send();

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

    switch (static_cast<ActionMsgType>(_messageType))
    {
    case ActionMsgType::PL_ATTACK_ROLL_INFO:
        AttackRollInfoReceived(_message, _sender);
        break;
    case ActionMsgType::PL_HIT_ROLL_INFO:
        HitRollInfoReceived(_message, _sender);
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
        // Tell the server that player cancelled
        if (Network::Messenger::IsServer() == false)
            Network::MsgActionEnd(this->GetID()).Send();

        // End the local action
        ActionPool::Instance().EndLocalAction();
    }
    else
    {
        if (Network::Messenger::IsServer() == false)
        {
            // Tell the server about the result
            Network::MsgActionInfo(this->GetID(), static_cast<uint8_t>(ActionMsgType::PL_ATTACK_ROLL_INFO), result).Send();
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
    return (_roll >= 15);
}

void Attack::AttackRollInfoReceived(const std::string& _message, uint8_t _sender)
{
    assert(Network::Messenger::IsServer());

    // TODO: query the GameMaster if he wants the attack to pass and reply!
    // TODO: how to find out about natural 20?

    int result = Utils::EvaluateFormula(_message, Game::RANDOM);

    if (EvaluateAttackRoll(result))
    {
        BroadcastHitMessage(_message, result);
        SendAttackRollHit(_sender);
    }
    else
    {
        BroadcastMissMessage(_message, result);
        SendAttackRollMissed(_sender);
    }
}

void Attack::AttackRollInfoLocal(const std::string& _message)
{
    assert(Network::Messenger::IsServer());

    // TODO: query the GameMaster if he wants the attack to pass and reply!
    // TODO: how to find out about natural 20?

    int result = Utils::EvaluateFormula(_message, Game::RANDOM);

    if (EvaluateAttackRoll(result))
    {
        BroadcastHitMessage(_message, result);
        AttackRollHit();
    }
    else
    {
        BroadcastMissMessage(_message, result);
        AttackRollMissed();
    }
}

void Attack::SendAttackRollHit(uint8_t _sender)
{
    // Tell client that he hit
    Network::MsgActionInfo(this->GetID(), static_cast<uint8_t>(ActionMsgType::DM_ATTACK_ROLL_HIT),
        STR_EMPTY).Send(_sender);
}

void Attack::SendAttackRollMissed(uint8_t _sender)
{
    // Tell client that he missed and end client action
    Network::MsgActionInfo(this->GetID(), static_cast<uint8_t>(ActionMsgType::DM_ATTACK_ROLL_MISS),
        STR_EMPTY).Send(_sender);
    Actions::ActionPool::Instance().EndClientAction(_sender);
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
    ActionPool::Instance().EndLocalAction();
}

void Attack::HitRollPromptFinished(States::GameState* gs)
{
    States::PromptState* ps = dynamic_cast<States::PromptState*>(gs);
    assert(ps && "Expected a PromptState on the top of the stack, wtf?");

    // Prompt is finished, what was the result?
    const std::string& result = ps->GetResult();
    if (result.empty())
    {
        // Tell the server that player cancelled
        if (Network::Messenger::IsServer() == false)
            Network::MsgActionEnd(this->GetID()).Send();

        // End the local action
        ActionPool::Instance().EndLocalAction();
    }
    else
    {
        if (Network::Messenger::IsServer() == false)
        {
            // Tell the server that about the result
            Network::MsgActionInfo(this->GetID(), static_cast<uint8_t>(ActionMsgType::PL_HIT_ROLL_INFO), result).Send();
        }
        else
        {
            // Tell myself about the result
            HitRollInfoLocal(result);
        }
    }
}

void Attack::HitRollInfoReceived(const std::string& _message, uint8_t _sender)
{
    // TODO: Implement
    // TODO: query the GameMaster if he is okay with the damage

    int result = Utils::EvaluateFormula(_message, Game::RANDOM);
    BroadcastDamageMessage(_message, result);

    // TODO: end client action and tell client about it
    Actions::ActionPool::Instance().EndClientAction(_sender);
}

void Attack::HitRollInfoLocal(const std::string& _message)
{
    // TODO: Implement
    // TODO: query the GameMaster if he is okay with the damage

    int result = Utils::EvaluateFormula(_message, Game::RANDOM);
    BroadcastDamageMessage(_message, result);
    Actions::ActionPool::Instance().EndLocalAction();
}

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
