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

        /// \brief Do whatever the action does.
        virtual void Execute() = 0;

    protected:
        std::vector<std::string> m_Requirements;
    };
}
