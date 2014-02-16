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

		/// \brief The actual code which is doing something.
		/// \details This method is public static because many other actions
		///		require a WalkTo before being applied.
		static void Perform(Core::ObjectID _executor, Core::ObjectID _target, bool _append = false);

		/// \brief Immediately end this action
		virtual bool Update() override { return true; }
    private:
        /// \brief Clones an attack action.
        virtual Action* Clone(Core::ObjectID _executor, Core::ObjectID _target);

        /// \brief Private constructor, fills requirements list.
        WalkTo();
    };
}