#pragma once

#include "Prerequisites.hpp"

namespace GameRules
{
    // \brief Provides all combat-specific calculations in the game
    // Let's try to keep it general and not implement too much.
    // Where necessary refer to http://paizo.com/pathfinderRPG/prd/combat.html
    class CombatRules
    {
    private:
        // \brief Provides only static functions, prohibit construction
        CombatRules() {}

    public:

        // \brief Contains information about a hit roll result
        struct HitRollInfo
        {
            // \brief Constructor for convenience
            HitRollInfo(bool hasHit, std::string message)
                : hasHit(hasHit), message(message) {}

            bool hasHit;          ///< Hit or miss
            std::string message;  ///< Reason for the hit/miss
        };

        // \brief Determine wether a target was hit by an attack
        // \param [in] target    Pointer to the object to be hit
        // \param [in] diceRoll  Value of the dice that was rolled (should be 1W20)
        // \param [in] modifier  Total modifier that is applied to the dice roll
        // \return HitRollInfo with true on hit, false on miss, and a message containing the reason
        // \throws Exception::NotEvaluatable if the AC is faulty
        HitRollInfo TargetHit(Core::ObjectID target, int diceRoll, int modifier);

        // \brief Returns the target's hit points
        // \param [in] target  Pointer to the object in question
        // \return target's current hit points
        // \throws Exception::NoSuchProperty if no HP property is found
        static int GetHitPoints(Core::ObjectID target);

        // \brief Returns the target's armor class.
        // \param [in] target  Pointer to the object in question
        // \return target's armor class
        // \throws Exception::NoSuchProperty if no HP property is found
        static int GetArmorClass(Core::ObjectID target);

        // \brief Returns the damage a hit does on an Object. This does NOT modify the target's hit points!
        // \param [in] target  Pointer to the object to be hit
        // \param [in] amount  Total result of the hit roll
        // \return damage that would be done if applied using ApplyHitDamage
        static int GetHitDamage(Core::ObjectID target, int amount);

        // \brief Apply hit damage to a target.
        // Calculates the damage using GetHitDamage() and subtracts it from the target's current hit points.
        // \param [in] target  Pointer to the object to be hit
        // \param [in] amount  Total result of the hit roll
        // \return The target's new hit points
        static int ApplyHitDamage(Core::ObjectID target, int amount);

    };
}