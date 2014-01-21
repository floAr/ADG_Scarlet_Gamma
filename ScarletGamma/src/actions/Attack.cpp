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

using namespace Actions;

Attack::Attack() : Action(STR_ACT_ATTACK)
{
    // Set requirements
    m_Requirements.push_back(GameRules::CombatRules::PROP_HITPOINTS);
    m_Requirements.push_back(GameRules::CombatRules::PROP_ARMORCLASS);
}

Action* Attack::Clone(Core::ObjectID target)
{
    Attack* result = new Attack();

    result->m_Target = target; // set target
    result->m_ID = m_ID; // copy ID

    return dynamic_cast<Action*>(result);
}

void Attack::Execute()
{
    // Tell the server that we are starting an attack
    if (Network::Messenger::IsServer() == false)
        Network::MsgActionBegin(this->GetID(), m_Target).Send();

    // Open prompt for hit roll value
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
    prompt->SetText("Angriffswurf eingeben:");
    prompt->AddPopCallback(std::bind(&Attack::AttackRollPromptFinished, this, std::placeholders::_1));
}

void Attack::HandleActionInfo(uint8_t messageType, const std::string& message, uint8_t sender)
{
    // I am the DM. This function handles player's requests and information.
    assert(Network::Messenger::IsServer());

    switch (static_cast<ActionMsgType>(messageType))
    {
    case ActionMsgType::PL_ATTACK_ROLL_INFO:
        AttackRollInfoReceived(message, sender);
        break;
    case ActionMsgType::PL_HIT_ROLL_INFO:
        HitRollInfoReceived(message, sender);
        break;
    default:
        assert(false);
    }
}

void Attack::HandleActionInfoResponse(uint8_t messageType, const std::string& message)
{
    // I am a player. This function handles DM's responses.
    assert(Network::Messenger::IsServer() == false);

    switch (static_cast<ActionMsgType>(messageType))
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
            // Tell the server that about the result
            Network::MsgActionInfo(this->GetID(), static_cast<uint8_t>(ActionMsgType::PL_ATTACK_ROLL_INFO), result).Send();
        }
        else
        {
            // Tell myself about the result
            AttackRollInfoLocal(result);
        }
    }
}

void Attack::AttackRollInfoReceived(const std::string& message, uint8_t sender)
{
    // TODO: query the GameMaster if he wants the attack to pass and reply!
    // TODO: how to find out about natural 20?

    //////////////////////////////////////////////////////////////////
    // TODO: PUT INTO A FUNCTION: EvaluateAttackRoll() or something //
    //////////////////////////////////////////////////////////////////

    // TODO: remove, I'm just testing ;)
    int result = Utils::EvaluateFormula(message, Game::RANDOM);
    bool hit = (result >= 15);

    if (hit)
    {
        // Tell client that he hit
        Network::MsgActionInfo(this->GetID(), static_cast<uint8_t>(ActionMsgType::DM_ATTACK_ROLL_HIT), std::to_string(result)).Send(sender);
    }
    else
    {
        // Tell client that he missed and end client action
        Network::MsgActionInfo(this->GetID(), static_cast<uint8_t>(ActionMsgType::DM_ATTACK_ROLL_MISS), std::to_string(result)).Send(sender);
        Actions::ActionPool::Instance().EndClientAction(sender);
        // ACTION IS DONE
    }
}

void Attack::AttackRollInfoLocal(const std::string& message)
{
    // TODO: query the GameMaster if he wants the attack to pass and reply!
    // TODO: how to find out about natural 20?

    //////////////////////////////////////////////////////////////////
    // TODO: PUT INTO A FUNCTION: EvaluateAttackRoll() or something //
    //////////////////////////////////////////////////////////////////

    // TODO: remove, I'm just testing ;)
    int result = Utils::EvaluateFormula(message, Game::RANDOM);
    bool hit = (result >= 15);

    if (hit)
        AttackRollHit();
    else
        AttackRollMissed();
}

void Attack::AttackRollMissed()
{
    // End local action
    ActionPool::Instance().EndLocalAction();
}

void Attack::AttackRollHit()
{
    // Prompt for damage
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
    prompt->SetText("Schadenswurf eingeben:");
    prompt->AddPopCallback(std::bind(&Attack::HitRollPromptFinished, this, std::placeholders::_1));
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

void Attack::HitRollInfoReceived(const std::string& message, uint8_t sender)
{
    // TODO: Implement
    // TODO: query the GameMaster if he is okay with the damage
}

void Attack::HitRollInfoLocal(const std::string& message)
{
    // TODO: Implement
    // TODO: query the GameMaster if he is okay with the damage
}