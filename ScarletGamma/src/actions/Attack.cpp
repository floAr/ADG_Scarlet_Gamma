#include "Attack.hpp"
#include "gamerules/CombatRules.hpp"

using namespace Actions;

Attack::Attack()
{
    // Set requirements
    m_Requirements.push_back(GameRules::CombatRules::PROP_HITPOINTS);
    m_Requirements.push_back(GameRules::CombatRules::PROP_ARMORCLASS);
}

void Attack::Execute()
{
    // This functions is called when a player clicks "Attack" on an object
    // What should happen next?

    // 1)  Check if the player is in proximity to the enemy or has a ranged weapon
    // 2)  Show a dialog to the player, asking for the Hit Roll values
    // 3)  Send the entered values to the Game Master
    //     Server side is handled elsewhere I hope?!
    //
    //     Server replies:
    // 4a) MISSED, were done. Kill the action!
    // 4b) CRIT CHANCE, roll again and send it to the server
    // 4c) HIT, enter damage (remember wether we did critical damage to show
    //     message "damage is auto-doubled") and send it to the server
    //
    // done!
}