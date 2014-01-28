#pragma once

#include "Action.hpp"
#include "core/Property.hpp"

namespace Actions
{
    class WalkTo : public Action
    {
        friend class ActionPool;

    public:
        /// \brief Initialize an attack action.
        virtual void Execute();

    private:
        /// \brief Clones an attack action.
        virtual Action* Clone(Core::ObjectID _excecutor, Core::ObjectID _target);

        /// \brief Private constructor, fills requirements list.
        WalkTo();
    };
}