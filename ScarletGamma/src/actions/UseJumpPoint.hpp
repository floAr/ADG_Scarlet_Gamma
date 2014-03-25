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

		/// \brief Server-side: handle action message, which always performs the jump.
		virtual void HandleActionInfo(uint8_t messageType, const std::string& message, uint8_t sender);

	private:
		/// \brief Clones an set jump point action.
		virtual Action* Clone(Core::ObjectID _executor, Core::ObjectID _target);

		/// \brief Actually performs the jump.
		void DoJump();

		/// \brief Private constructor, fills requirements list.
		UseJumpPoint();
	};
}