#pragma once

#include "Action.hpp"
#include "core/Property.hpp"

namespace Actions
{
	class UseJumpPoint : public Action
	{
		friend class ActionPool;

	public:
		/// \brief Perform a WalkTo if necessary and than jump.
		virtual void Execute() override;

		/// \brief Check if we are close enough to do the jump.
		/// \details This ends the action.
		virtual void Update() override;

	private:
		/// \brief Clones an set jump point action.
		virtual Action* Clone(Core::ObjectID _executor, Core::ObjectID _target);

		/// \brief Private constructor, fills requirements list.
		UseJumpPoint();
	};
}