#include "Combat.hpp"
#include "Game.hpp"
#include "States/StateMachine.hpp"
#include "states/PromptState.hpp"

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
    // TODO: we need to do something with the object yo!

    States::PromptState* prompt = dynamic_cast<States::PromptState*>(
        g_Game->GetStateMachine()->PushGameState(States::GST_PROMPT));
    prompt->SetText("Angriffswurf eingeben:");
    //prompt->AddPopCallback(std::bind(0x70D0, this, std::placeholders::_1));
}

//void States::PlayerState::InitiativeRollPromptFinished( States::GameState* _gs )
//{
//    PromptState* prompt = dynamic_cast<PromptState*>(ps);
//    assert(ps);
//
//    // Send initiative roll string to server
//    Jo::Files::MemFile data;
//    const std::string& result = prompt->GetResult().c_str();
//    data.Write(&m_selection[0], sizeof(m_selection[0]));
//    data.Write(result.c_str(), result.length());
//    Network::CombatMsg(Network::CombatMsgType::PL_COMBAT_INITIATIVE).Send(&data);
//}
