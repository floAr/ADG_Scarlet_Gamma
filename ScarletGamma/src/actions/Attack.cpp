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

void Attack::HandleActionInfo(uint8_t messageType, const std::string& message)
{
    switch (static_cast<ActionMsgType>(messageType))
    {
    case ActionMsgType::PL_ATTACK_ROLL_INFO:
        AttackRollInfoReceived(message);
        break;
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
    case ActionMsgType::PL_HIT_ROLL_INFO:
        HitRollInfoReceived(message);
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
            AttackRollInfoReceived(result);
        }
    }
}

void Attack::AttackRollInfoReceived(const std::string& message)
{
    // TODO: query the GameMaster if he wants the attack to pass and reply!
    // TODO: how to find out about natural 20?

    // TODO: remove, I'm just testing ;)
    int result = Utils::EvaluateFormula(message, Game::RANDOM);
    if (result >= 15)
    {
        // Tell the player that he hit
        Network::MsgActionInfo(this->GetID(), static_cast<uint8_t>(ActionMsgType::DM_ATTACK_ROLL_HIT), std::to_string(result)).Send();
    }
    else
    {
        // Tell the player that he missed
        Network::MsgActionInfo(this->GetID(), static_cast<uint8_t>(ActionMsgType::DM_ATTACK_ROLL_MISS), std::to_string(result)).Send();
    }
}

void Attack::AttackRollMissed()
{
    // TODO: Implement
}

void Attack::AttackRollHit()
{
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
            HitRollInfoReceived(result);
        }
    }
}

void Attack::HitRollInfoReceived(const std::string& message)
{
    // TODO: Implement
    // TODO: query the GameMaster if he is okay with the damage
}