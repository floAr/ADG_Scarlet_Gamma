#pragma once

#include "Combat.hpp"
#include <unordered_map>

namespace GameRules
{
    class MasterCombat : public Combat
    {
    public:
        /// \brief Adds a combattant to the combat. May happen before or during
        ///     the combat.
        /// \details This requests an initiative roll from the specified object.
        ///     When the result is received, ReceivedInitiative is called.
        void AddParticipant(Core::ObjectID _object);

        /// \brief Adds a combat participant to the correct position in the
        ///     initiavite list.
        void ReceivedInitiative(Core::ObjectID _object, std::string& _initiative);

        /// \brief Called when a DM ini prompt is done.
        virtual void InitiativeRollPromptFinished( States::GameState* _ps, Core::ObjectID _object ) override;

        /// \brief Called when the DM added the (initial) participants and wants
        ///     to start the combat.
        void StartCombat();

        /// \brief Sets the object whose turn it is next.
        virtual void SetTurn(Core::ObjectID _object) override;

    protected:
        /// \brief Keeps the initiative for each combat participant.
        std::unordered_map<Core::ObjectID, int> m_initiatives;
    };
}
