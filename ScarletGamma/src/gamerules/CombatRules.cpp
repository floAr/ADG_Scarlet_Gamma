#include "gamerules/CombatRules.hpp"
#include "Constants.hpp"
#include "core/Object.hpp"
#include "core/Property.hpp"
#include "utils/Exception.hpp"
#include "Game.hpp"
#include "core/World.hpp"
#include <string>
#include <assert.h>

using namespace GameRules;
using namespace std;

CombatRules::HitRollInfo CombatRules::TargetHit(Core::ObjectID target, int diceRoll, int modifier)
{
    Core::Object* targetObj = g_Game->GetWorld()->GetObject(target);
    assert(target);

    // Target needs an AC to be hittable
    if (targetObj->HasProperty(STR_PROP_ARMORCLASS) == false)
        return HitRollInfo(false, STR_MSG_HIT_NO_ARMOR_CLASS);

    // Evaluate AC, may throw an NotEvaluatable exception
    int AC = targetObj->GetProperty(STR_PROP_ARMORCLASS).Evaluate();

    // Always hit on natural 20
    if (diceRoll == 20)
        return HitRollInfo(true, STR_MSG_HIT_NATURAL_TWENTY);

    // Hit if AC is achieved
    if (diceRoll + modifier >= AC)
        return HitRollInfo(true, STR_MSG_HIT_ARMOR_CLASS_BEAT);

    // Don't hit otherwise
    return HitRollInfo(false, STR_MSG_HIT_ARMOR_CLASS_NOT_BEAT);
}

int CombatRules::GetHitPoints(Core::ObjectID target)
{
    Core::Object* targetObj = g_Game->GetWorld()->GetObject(target);
    assert(target);

    // Target needs HP, otherwise throw exception
    if (targetObj->HasProperty(STR_PROP_HEALTH) == false)
        throw Exception::NoSuchProperty();

    // Evaluate HP, may throw an NotEvaluatable exception
    return targetObj->GetProperty(STR_PROP_HEALTH).Evaluate();
}

int CombatRules::GetArmorClass(Core::ObjectID target)
{
    Core::Object* targetObj = g_Game->GetWorld()->GetObject(target);
    assert(target);

    // Target needs HP, otherwise throw exception
    if (targetObj->HasProperty(STR_PROP_ARMORCLASS) == false)
        throw Exception::NoSuchProperty();

    // Evaluate HP, may throw an NotEvaluatable exception
    return targetObj->GetProperty(STR_PROP_ARMORCLASS).Evaluate();
}

int CombatRules::GetHitDamage(Core::ObjectID target, int amount)
{
    Core::Object* targetObj = g_Game->GetWorld()->GetObject(target);
    assert(target);

    // For now, just return the amount
    // TODO: resistencies, blocking etc.
    return amount;
}

int CombatRules::ApplyHitDamage(Core::ObjectID target, int amount)
{
    Core::Object* targetObj = g_Game->GetWorld()->GetObject(target);
    assert(target);

    // Target needs HP, otherwise throw exception
    if (targetObj->HasProperty(STR_PROP_HEALTH) == false)
        throw Exception::NoSuchProperty();

    // Try to set the new HP value
    Core::Property& HPprop = targetObj->GetProperty(STR_PROP_HEALTH);
    int newHP = HPprop.Evaluate() - GetHitDamage(target, amount);
    HPprop.SetValue(to_string(newHP));

    // Return new HP
    return newHP;
}