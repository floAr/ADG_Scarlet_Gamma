#pragma once

#include "Action.hpp"
#include "Prerequisites.hpp"

namespace Actions
{
    class Attack : public Action
    {
        friend class ActionPool;

    public:
        /// \brief Initialize an attack action.
        virtual void Execute();

        void HitRollPromptFinished(States::GameState* gs);

    private:
        /// \brief Clones an attack action.
        virtual Action* Clone();

        /// \brief Private constructor, fills requirements list.
        Attack();
    };
}