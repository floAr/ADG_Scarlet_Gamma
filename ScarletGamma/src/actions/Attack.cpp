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
    // TODO
}