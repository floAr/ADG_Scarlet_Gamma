#include "Combat.hpp"
#include "Game.hpp"
#include "States/StateMachine.hpp"
#include "states/PromptState.hpp"
#include "network/CombatMessages.hpp"
#include "Constants.hpp"
#include "core/World.hpp"
#include <jofilelib.hpp>

using namespace GameRules;

void Combat::AddParticipantWithInitiative(Core::ObjectID _object, int8_t _position)
{
    // Use user-defined index: negative values count backwards from the end
    // of the list
    uint8_t size;
    if (_position < 0)
        size = m_participants.size() + _position;
    else
        size = _position;

    // Iterate to position
    auto it = m_participants.begin();
    for (uint8_t i = 0; i < size && it != m_participants.end(); ++it);

    // insert object at position
    m_participants.insert( it, _object );
}

void GameRules::Combat::PushInitiativePrompt( Core::ObjectID _object )
{
    // Push initiative prompt state
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(
        g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
    Core::Object* object = g_Game->GetWorld()->GetObject(_object);
    prompt->SetText("Initiativewurf für " + object->GetName() + " eingeben:");

    // Set the default value for the roll
    if (object->HasProperty(STR_PROP_INITIATIVE_MOD))
        prompt->SetDefaultValue("1W20 + '" + STR_PROP_INITIATIVE_MOD + "'");
    else
        prompt->SetDefaultValue("1W20");

    // Add callback function
    prompt->AddPopCallback(std::bind(&Combat::InitiativeRollPromptFinished, this,
        std::placeholders::_1, _object));
}

void GameRules::Combat::InitiativeRollPromptFinished( States::GameState* _ps, Core::ObjectID _object )
{
    States::PromptState* prompt = dynamic_cast<States::PromptState*>(_ps);
    assert(prompt);

    // Send initiative roll string to server
    Jo::Files::MemFile data;
    const std::string& result = prompt->GetResult().c_str();
    data.Write(&_object, sizeof(_object));
    data.Write(result.c_str(), result.length());
    Network::CombatMsg(Network::CombatMsgType::PL_COMBAT_INITIATIVE).Send(&data);
}
