#include "MasterCombat.hpp"
#include "Game.hpp"
#include "core/World.hpp"
#include "utils/ValueInterpreter.hpp"
#include "Constants.hpp"
#include "utils/Random.hpp"
#include "network/CombatMessages.hpp"
#include <iostream>

using namespace GameRules;
using namespace Network;

void GameRules::MasterCombat::AddParticipant( Core::ObjectID _object )
{
    if (g_Game->GetWorld()->GetObject(_object)->HasProperty(STR_PROP_OWNER))
    {
        // Send message with ObjectID to all players. Each player then checks
        // whether the object belongs to him and if so provides an initiative roll.
        Jo::Files::MemFile data;
        data.Write(&_object, sizeof(_object));
        Network::CombatMsg(Network::CombatMsgType::DM_COMBAT_BEGIN).Send(&data);
    }
    else
    {
        // No owner, so the DM needs to take care of the poor guy
        PushInitiativePrompt(_object);
    }
}

void MasterCombat::ReceivedInitiative(Core::ObjectID _object, std::string& _initiative)
{
    // Save initiative value
    int iniEvaluated = Utils::EvaluateFormula(_initiative, Game::RANDOM, g_Game->GetWorld()->GetObject(_object));
    m_initiatives.insert( std::pair<Core::ObjectID, int>( _object, iniEvaluated) );

    // Insert combattant into list of participants
    auto it = m_participants.begin();
    while (it != m_participants.end())
    {
        // Check its initiative value
        if (m_initiatives[*it] > iniEvaluated)
        {
            // Better than me. Nnnnnext!
            ++it;
        }
        else
        {
            if (m_initiatives[*it] < iniEvaluated)
            {
                // I'm before that object
                break;
            }
            else
            {
                // Initiative values are equal, compare ini mod
                Core::Object* me = g_Game->GetWorld()->GetObject(_object);
                Core::Object* rival = g_Game->GetWorld()->GetObject(*it);
                if ( me->HasProperty(STR_PROP_INITIATIVE_MOD) && rival->HasProperty(STR_PROP_INITIATIVE_MOD) )
                {
                    int myIniMod = me->GetProperty(STR_PROP_INITIATIVE_MOD).Evaluate();
                    int rivalIniMod = rival->GetProperty(STR_PROP_INITIATIVE_MOD).Evaluate();
                    if ( rivalIniMod > myIniMod )
                    {
                        // Rival is better, put me behind him
                        ++it;
                    }
                    else
                    {
                        // Same value?
                        if ( rivalIniMod == myIniMod )
                        {
                            if (Game::RANDOM->Uniform(0, 1) != 0)
                                ++it;
                        }
                    }
                }

                break;
            }
        }
    }

    // Tell all clients to insert participant
    int8_t position = std::distance(m_participants.begin(), it);
    Jo::Files::MemFile data;
    data.Write(&_object, sizeof(_object));
    data.Write(&position, sizeof(position));  
    CombatMsg(CombatMsgType::DM_COMBAT_ADD_PARTICIPANT).Send(&data);

    // Insert object before it
    m_participants.insert(it, _object);

#ifdef _DEBUG
    std::cout << "Object " << _object << " rolled initiative " << iniEvaluated <<
        ", inserted at position " << std::to_string(position) << ".\n";
#endif
}

