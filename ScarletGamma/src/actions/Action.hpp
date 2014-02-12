#pragma once

#include "Prerequisites.hpp"
#include "Game.hpp"
#include "core/Object.hpp"
#include "core/World.hpp"
#include <vector>
#include <cstdint>

namespace Actions
{
    /// \brief Determines the duration when used in combat.
    enum struct Duration : uint8_t
    {
        STANDARD_ACTION, ///< Do something, most commonly to make an attack or cast a spell
        MOVE_ACTION,     ///< Move up to your speed or perform an action that takes a similar amount of time
        FULL_ACTION,     ///< Consumes all your effort during a round
        FREE_ACTION,     ///< Consume a very small amount of time and effort
        SWIFT_ACTION     ///< Consumes a very small amount of time, but more than a free action
    };

    class Action
    {
        friend class ActionPool;

    public:
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
        Action(const std::string& name, Duration type, int priority, Game::MouseCursor cursor = Game::MC_DEFAULT) :
            m_name(name), m_duration(type), m_priority(priority), m_cursor(cursor), m_finished(true) {}

        /// \brief Returns true if this action can be used right now. Overwrite to
        ///     define other conditions.
        virtual bool CanUse(Core::ObjectList& _executors, Core::Object& _object)
        {
            // Get all requirements for target and loop through them
            const std::vector<std::pair<std::string, bool>>& requirements = m_targetRequirements;
            for (auto req = requirements.begin(); req != requirements.end(); ++req)
                if (_object.HasProperty((*req).first) != (*req).second)
                    return false;

            // Get all requirements for executor and loop through them
            const std::vector<std::pair<std::string, bool>>& sourceRequirements = m_sourceRequirements;
            for (auto req = sourceRequirements.begin(); req != sourceRequirements.end(); ++req)
                // Test all possible executors
                for( int i=0; i<_executors.Size(); ++i )
                    if( g_Game->GetWorld()->GetObject(_executors[i])->HasProperty((*req).first) != (*req).second )
                        return false;

            return true;
        }

        std::vector<std::pair<std::string, bool>> m_targetRequirements; ///< List of required properties in target
        std::vector<std::pair<std::string, bool>> m_sourceRequirements; ///< List of required properties in source (executor)
        const std::string m_name; ///< Human readable name of the action
        Duration m_duration; ///< Duration of the action, i.e. how long the action will take in combat
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
