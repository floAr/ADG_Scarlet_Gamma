#pragma once

#include "Prerequisites.hpp"
#include "actions/Action.hpp"
#include <cstdint>
#include <list>

namespace GameRules
{
    class Combat
    {
    public:
		/// \brief Sets default values
		Combat() : m_currentObject(nullptr), m_diagonalCounter(false),
			m_fiveFootStepRemaining(true), m_moveActionRemaining(true),
			m_moveActionStepsLeft(9.0f), m_standardActionRemaining(true) {}

        /// \brief Adds a participant to the combat.
        /// \detail By default, this function adds a participant to the end of
        ///     the round. You can add a participant counting from the front
        ///     using positive values (0 = before first) or from the back of the
        ///     list using negative values (-1 = after last, -2 = before last...)
        void AddParticipantWithInitiative(Core::ObjectID _object, int8_t _position = -1);

		/// \brief Checks whether the object already is in combat.
		bool HasParticipant(Core::ObjectID _object) const;

        /// \brief Creates an initiative prompt window, using the specified object
        ///     as reference object for property values.
        void PushInitiativePrompt(Core::ObjectID _object);

        /// \brief Is called when the initiative roll is accepted.
		virtual void InitiativeRollPromptFinished(std::string& _result, Core::Object* _object);

        /// \brief Is called when it is a combattant's turn.
        virtual void SetTurn( Core::ObjectID _object );

        /// \brief I'm done, end my turn.
        virtual void EndTurn();

        /// \brief Returns true if the action can still be used in this round.
        bool CanUse(Actions::Duration _duration) const;

        /// \brief Returns the number of steps that can be used in this round.
        float GetRemainingSteps() const;

        /// \brief Sets the standard action as used.
        void UseStandardAction();

        /// \brief Sets the move action as used. If a distance is specifiedm, the
        ///     5-foot-step will also be disabled.
        bool UseMoveAction(float _distance = 0, bool _diagonal = false);

        /// \brief Returns the current object's ID.
        Core::ObjectID GetTurn() const;
		
		/// \brief Checks whether the combat has started yet.
		bool HasStarted() const;

		/// \brief Updates the combatant panel.
		void UpdateCombatantPanel();

		/// \brief Updates the combatant panel.
		void UpdateCombatantPanelTurn();

    protected:
        /// \brief Map of participants, sorted by their order of initiative
        std::list<Core::ObjectID> m_participants;

        /// \brief The object whose turn it currently is.
        Core::Object* m_currentObject;

        /// \brief Keeps whether the move action is still available in this turn.
        bool m_moveActionRemaining;

        /// \brief Counts the remaining steps of a move action.
        /// \details Defaults to the object's base speed and is decreased with every step,
        ///     or set to 0 if an action consumes the move action altogether.
        float m_moveActionStepsLeft;

        /// \brief Keeps whether the standard action is still available in this turn.
        bool  m_standardActionRemaining;

        /// \brief When you don't move any distance in a round, you can still take a 5-foot-
        ///     step. You will hear me calling this thing "Gratis-Schritt".
        /// \details Note that 0-distance move actions (e.g. load crossbow) don't prevent this.
        bool m_fiveFootStepRemaining;

        /// \brief Counts the diagonal moves. Every second diagonal move counts as 3m step,
        ///     so a bool is sufficient.
        bool m_diagonalCounter;
    };
}
