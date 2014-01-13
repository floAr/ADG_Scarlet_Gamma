#pragma once

#include "Action.hpp"
#include "core/Property.hpp"

namespace Actions
{
    class Attack : public Action
    {
    public:
        /// \brief Returns the list of required properties for this action.
        /// \return  Reference to list of requirements
        virtual const std::string* GetRequirements()
        {
            return REQUIREMENTS;
        }

        /// \brief Initialize an attack action.
        virtual void Execute();

    private:
        static const std::string REQUIREMENTS[];
    };
}