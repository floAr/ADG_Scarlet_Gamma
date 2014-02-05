#pragma once

#include "Action.hpp"
#include "core/Property.hpp"

namespace Actions
{
	class SetJumpPoint : public Action
	{
		friend class ActionPool;

	public:
		/// \brief Set a target for the jump point.
		virtual void Execute();

		/// \brief Immediately end this action
		virtual bool Update() override { return true; }
	private:
		/// \brief Clones an set jump point action.
		virtual Action* Clone(Core::ObjectID _executor, Core::ObjectID _target);

		/// \brief Private constructor, fills requirements list.
		SetJumpPoint();
	};
}