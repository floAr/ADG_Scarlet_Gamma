#pragma  once

#include "Prerequisites.hpp"

namespace Interfaces {

	/// \brief All drag and drop events must fill or interpret the data
	///		in this container.
	///	\details This is primary designed for Object and Property-GUI
	///		changes. Dragging other thinks is not intended.
	struct DragContent
	{
		enum Sources {
			PROPERTY_PANEL,
			MODULES_PANEL,
			OBJECT_PANEL,
			PLAYERS_LIST
		};

		Sources from;				///< How filled that object?
		const Core::Object* object;	///< The object which is dragged or the object which contains the property which is dragged.
		const Core::Property* prop;	///< nullptr or a property which was below the cursor on drag start

		DragContent() : object(nullptr), prop(nullptr)	{}
	};

} // namespace Interfaces