/// \file Prerequisites.h
/// Contains forward declarations for all / most of the classes.

#pragma once

//------------------------------------------------------------------------------
// GLOBAL NAMESPACE
class Game;

//------------------------------------------------------------------------------
// FUNDAMENTALS
namespace Fundamentals
{
	// todo...
}

namespace Exception {
	class NotEvaluateable;
	class NoObjectList;
	class NoSuchProperty;
}

//------------------------------------------------------------------------------
// CORE
namespace Core
{
	class ObjectList;
	class Property;
	class PropertyList;

	/// \brief General definition of an object handle.
	/// \details This typedef allows to introduce 64Bit or even complexer types
	///		if necessary.
	typedef unsigned int ObjectID;
}

//------------------------------------------------------------------------------
// STATES
namespace States
{
	class StateMachine;
	class GameState;
}

//------------------------------------------------------------------------------
// GRAPHICS
namespace Graphics
{
}