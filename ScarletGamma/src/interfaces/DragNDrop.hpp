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
			PLAYERS_LIST,
			MAP
		};

		Sources from;				///< How filled that object?
		Core::Object* object;		///< The object which is dragged or the object which contains the property which is dragged.
		Core::Property* prop;		///< nullptr or a property which was below the cursor on drag start
		Core::ObjectList* manyObjects;	///< Filled if a selection is moved by the map

		DragContent() : object(nullptr), prop(nullptr), manyObjects(nullptr)	{}
	};

} // namespace Interfaces