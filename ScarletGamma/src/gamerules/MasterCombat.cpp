#include "MasterCombat.hpp"
#include "Game.hpp"
#include "core/World.hpp"
#include "utils/ValueInterpreter.hpp"
#include "Constants.hpp"
#include "utils/Random.hpp"

using namespace GameRules;

void MasterCombat::ReceivedInitiative(Core::ObjectID _object, std::string& _initiative)
{
    // Save initiative value
    int iniEvaluated = Utils::EvaluateFormula(_initiative, Game::RANDOM);
    m_initiatives.insert( std::pair<Core::ObjectID, int>( _object, iniEvaluated) );

    // Insert combattant into list of participants
    auto it = m_participants.begin();
    while (it != m_participants.end())
    {
        // Check its initiative value
        if (m_initiatives[*it] > iniEvaluated)
        {
            // Nnnnnext!
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

    // Insert object before it
    m_participants.insert(it, _object);
}
