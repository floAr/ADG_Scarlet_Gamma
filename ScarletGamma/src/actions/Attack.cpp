#include "Attack.hpp"
#include "Game.hpp"
#include "Constants.hpp"
#include "gamerules/CombatRules.hpp"
#include "network/ActionMessages.hpp"
#include "states/StateMachine.hpp"
#include "states/PromptState.hpp"

using namespace Actions;

Attack::Attack() : Action(STR_ACT_ATTACK)
{
    // Set requirements
    m_Requirements.push_back(GameRules::CombatRules::PROP_HITPOINTS);
    m_Requirements.push_back(GameRules::CombatRules::PROP_ARMORCLASS);
}

void Attack::Execute()
{
    // Tell the server that we are starting an attack
    Network::MsgActionBegin(this->GetID()).Send();

    // Open prompt for hit roll value
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
    prompt->SetText("Angriffswurf eingeben:");
    prompt->AddPopCallback(std::bind(&Attack::HitRollPromptFinished, this, std::placeholders::_1));
}

void Attack::HitRollPromptFinished(States::GameState* gs)
{
    States::PromptState* ps = dynamic_cast<States::PromptState*>(gs);
    assert(ps);

    // Prompt is finished, what was the result?
    const std::string& result = ps->GetResult();
    if (result.empty())
    {
    }
    std::cout << "Hit roll prompt state finished! Entered was " << (result.empty()? "nothing" : result)
        << std::endl;

    // TODO: read hit roll value from promptState and process...
}

Action* Attack::Clone()
{
    Attack* result = new Attack();

    // TODO: copy all values
    result->m_ID = m_ID;

    return dynamic_cast<Action*>(result);
}