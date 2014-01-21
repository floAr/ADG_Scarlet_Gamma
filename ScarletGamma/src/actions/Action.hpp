#pragma once

#include "Prerequisites.hpp"
#include <vector>
#include <cstdint>

namespace Actions
{
    class Action
    {
        friend class ActionPool;

    public:
        /// \brief Returns the list of required properties for this action.
        /// The respective implementations needs to take care of filling the list.
        /// \return  Reference to list of requirements
        const std::vector<std::string>& GetRequirements()
        {
            return m_Requirements;
        }

        /// \brief Get the name of an action for human readability.
        /// \return Const reference to name string
        virtual const std::string& GetName()
        {
            return m_Name;
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
        /// \brief Do whatever the action does.
        virtual void Execute() = 0;

        /// \brief Create a copy of the action. Needs to AT LEAST copy the
        ///   Action ID and the target.
        virtual Action* Clone(Core::ObjectID target) = 0;

        /// \brief Default constructor, setting the name of the action
        Action(const std::string& name) : m_Name(name) {}

        /// \brief Get the ID of the action, mainly for networking
        /// \return ID of the action
        Core::ActionID GetID()
        {
            return m_ID;
        }

        std::vector<std::string> m_Requirements; ///< List of required properties for this action
        const std::string m_Name; ///< Human readable name of the action
        Core::ActionID m_ID; ///< Action ID, i.e. index in the ActionPool's list
        Core::ObjectID m_Target; ///< The target of the action

    private:

        /// \brief Prevent copying.
        Action(const Action& other) {}

        /// \brief Sets the ID of the actual. Should only be called by the ActionPool!
        /// \param [in] _id  The ID of the action, i.e. the index in the ActionPool
        void SetID(Core::ActionID _id)
        {
            m_ID = _id;
        }
    };
}
