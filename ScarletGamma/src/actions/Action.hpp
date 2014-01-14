#pragma once

#include "Prerequisites.hpp"

namespace Actions
{
    class Action
    {
    public:
        /// \brief Returns the list of required properties for this action.
        /// The respective implementations need to take care of the list
        /// variable itself.
        /// \return  Reference to list of requirements
        virtual const std::string* GetRequirements() = 0;

        /// \brief Do whatever the action does.
        virtual void Execute() = 0;
    };
}
