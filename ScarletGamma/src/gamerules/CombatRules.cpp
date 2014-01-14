#include "gamerules/CombatRules.hpp"
#include "Constants.hpp"
#include "core/Object.hpp"
#include "core/Property.hpp"
#include "utils/Exception.hpp"
#include <string>
#include <assert.h>

using namespace GameRules;
using namespace std;

const string CombatRules::PROP_ARMORCLASS = string("armorclass");
const string CombatRules::PROP_HITPOINTS = string("hitpoints");

CombatRules::HitRollInfo CombatRules::TargetHit(Core::Object* target, int diceRoll, int modifier)
{
    // We really need a target object
    assert(target);

    // Target needs an AC to be hittable
    if (target->HasProperty(PROP_ARMORCLASS) == false)
        return HitRollInfo(false, STR_HIT_NO_ARMOR_CLASS);

    // Evaluate AC, may throw an NotEvaluatable exception
    int AC = target->GetProperty(PROP_ARMORCLASS).Evaluate();

    // Always hit on natural 20
    if (diceRoll == 20)
        return HitRollInfo(true, STR_HIT_NATURAL_TWENTY);

    // Hit if AC is achieved
    if (diceRoll + modifier >= AC)
        return HitRollInfo(true, STR_HIT_ARMOR_CLASS_BEAT);

    // Don't hit otherwise
    return HitRollInfo(false, STR_HIT_ARMOR_CLASS_NOT_BEAT);
}

int CombatRules::GetHitPoints(Core::Object* target)
{
    // We really need a target object
    assert(target);

    // Target needs HP, otherwise throw exception
    if (target->HasProperty(PROP_HITPOINTS) == false)
        throw Exception::NoSuchProperty();

    // Evaluate HP, may throw an NotEvaluatable exception
    return target->GetProperty(PROP_HITPOINTS).Evaluate();
}

int CombatRules::GetHitDamage(Core::Object* target, int amount)
{
    // We really need a target object
    assert(target);

    // For now, just return the amount
    // TODO: resistencies, blocking etc.
    return amount;
}

int CombatRules::ApplyHitDamage(Core::Object* target, int amount)
{
    // We really need a target object
    assert(target);

    // Target needs HP, otherwise throw exception
    if (target->HasProperty(PROP_HITPOINTS) == false)
        throw Exception::NoSuchProperty();

    // Try to set the new HP value
    Core::Property& HPprop = target->GetProperty(PROP_HITPOINTS);
    int newHP = HPprop.Evaluate() - GetHitDamage(target, amount);
    HPprop.SetValue(to_string(newHP));

    // Return new HP
    return newHP;
}