#pragma once

#include "Prerequisites.hpp"
#include "Game.hpp"
#include <vector>
#include <cstdint>

namespace Actions
{
    enum struct ActionType : uint8_t
    {
        STANDARD_ACTION, ///< Do something, most commonly to make an attack or cast a spell
        MOVE_ACTION,     ///< Move up to your speed or perform an action that takes a similar amount of time
        FULL_ACTION,     ///< Consumes all your effort during a round
        FREE_ACTION,     ///< Consume a very small amount of time and effort
        SWIFT_ACTION,    ///< Consumes a very small amount of time, but more than a free action
        NOT_IN_COMBAT    ///< Only when exectutor is not in combat
    };

    class Action
    {
        friend class ActionPool;

    public:
        /// \brief Returns the list of required properties for the target for this action.
        /// The respective implementations needs to take care of filling the list.
        /// \return  Reference to list of requirements
        const std::vector<std::pair<std::string, bool>>& GetTargetRequirements()
        {
            return m_targetRequirements;
        }

        /// \brief Returns the list of required properties for the executor.
        /// The respective implementations needs to take care of filling the list.
        /// \return  Reference to list of source requirements
        const std::vector<std::pair<std::string, bool>>& GetSourceRequirements()
        {
            return m_sourceRequirements;
        }

        /// \brief Get the name of an action for human readability.
        /// \return Const reference to name string
        virtual const std::string& GetName()
        {
            return m_name;
        }

        /// \brief Client-side: handle an action message in whatever way is appropriate.
        /// \detail You only need to overwrite this if the action to be implemented
        ///   requires some form of action-specific network communication.
        virtual void HandleActionInfo(uint8_t messageType, const std::string& message, uint8_t sender) {}

        /// \brief Client-side: handle an action message in whatever way is appropriate.
        /// \detail You only need to overwrite this if the action to be implemented
        ///   requires some form of action-specific network communication.
        virtual void HandleActionInfoResponse(uint8_t messageType, const std::string& message) {}

    protected:
        /// \brief Do whatever the action does at startup.
        virtual void Execute() = 0;

		/// \brief Continue some active actions.
		/// \return true if the action was performed and can be ended.
		virtual bool Update() { return m_finished; }

        /// \brief Create a copy of the action. Needs to AT LEAST copy the
        ///   Action ID, executor and target, action type, priority and cursor.
        virtual Action* Clone(Core::ObjectID _executor, Core::ObjectID target) = 0;

        /// \brief Default constructor, setting some required variables
        Action(const std::string& name, ActionType type, int priority, Game::MouseCursor cursor = Game::MC_DEFAULT) :
            m_name(name), m_actionType(type), m_priority(priority), m_cursor(cursor), m_finished(true) {}

        std::vector<std::pair<std::string, bool>> m_targetRequirements; ///< List of required properties in target
        std::vector<std::pair<std::string, bool>> m_sourceRequirements; ///< List of required properties in source (executor)
        const std::string m_name; ///< Human readable name of the action
        ActionType m_actionType; ///< Type of the action, i.e. how long the action will take in combat
        Core::ActionID m_id; ///< Action ID, i.e. index in the ActionPool's list
        Core::ObjectID m_executor; ///< The object executing the action
        Core::ObjectID m_target; ///< The object being targeted by the action
        int m_priority; ///< The priority of the action, used for determining default actions
        Game::MouseCursor m_cursor; ///< The cursor displayed when this action is the default action
        bool m_finished; ///< Set to true when the action is finished.

    private:

        /// \brief Prevent copying.
        Action(const Action& other) {}
    };
}
