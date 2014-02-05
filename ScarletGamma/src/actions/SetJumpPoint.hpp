#pragma once

#include "Action.hpp"
#include "core/Property.hpp"

namespace Actions
{
	class SetJumpPoint : public Action
	{
		friend class ActionPool;

	public:
		/// \brief Initialize an set jump point action.
		virtual void Execute();

	private:
		/// \brief Clones an set jump point action.
		virtual Action* Clone(Core::ObjectID _executor, Core::ObjectID _target);

		/// \brief Private constructor, fills requirements list.
		SetJumpPoint();
	};
}