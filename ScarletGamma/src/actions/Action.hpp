#pragma once

#include "Prerequisites.hpp"
#include <vector>

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

        /// \brief Do whatever the action does.
        virtual void Execute() = 0;

    protected:
        /// \brief Default constructor, setting the name of the action
        Action(std::string name) : m_Name(name) {}

        std::vector<std::string> m_Requirements; ///< List of required properties for this action
        const std::string m_Name; ///< Human readable name of the action
    };
}
