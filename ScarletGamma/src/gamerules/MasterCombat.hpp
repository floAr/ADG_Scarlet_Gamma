#pragma once

#include "Combat.hpp"
#include <unordered_map>

namespace GameRules
{
    class MasterCombat : public Combat
    {
    public:
		/// \brief Tells all clients about its deletion.
		~MasterCombat();

        /// \brief Adds a combattant to the combat. May happen before or during
        ///     the combat.
        /// \details This requests an initiative roll from the specified object.
        ///     When the result is received, ReceivedInitiative is called.
        void AddParticipant(Core::ObjectID _object);

        /// \brief Adds a combat participant to the correct position in the
        ///     initiavite list.
        void ReceivedInitiative(Core::ObjectID _object, std::string& _initiative);

        /// \brief Called when a DM ini prompt is done.
        virtual void InitiativeRollPromptFinished(std::string& _result, Core::Object* _object) override;

        /// \brief Called when the DM added the (initial) participants and wants
        ///     to start the combat.
        void StartCombat();

		/// \brief Tells all players to end the combat.
		void EndCombat();

        /// \brief Sets the object whose turn it is next.
        virtual void SetTurn(Core::ObjectID _object) override;

        /// \brief Called when a request to end the current turn is received.
        void ReceivedTurnEnded();

        /// \brief Sets the next combattant's turn.
        virtual void EndTurn() override;

    protected:
        /// \brief Keeps the initiative for each combat participant.
        std::unordered_map<Core::ObjectID, int> m_initiatives;
    };
}
