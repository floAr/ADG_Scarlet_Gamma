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
	class Object;
	class Map;
	class World;

	/// \brief General definition of an object handle.
	/// \details This typedef allows to introduce 64Bit or even complexer types
	///		if necessary.
	typedef unsigned int ObjectID;

	/// \brief A map handle inside the world.
	typedef unsigned int MapID;
}

//------------------------------------------------------------------------------
// EVENTS
namespace Events
{
	class EventHandler;
	class InputHandler;
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
	class TileRenderer;
}

//------------------------------------------------------------------------------
// MATHEMATICS
#include "SFML/System/Vector2.hpp"

//------------------------------------------------------------------------------
// TEST
namespace UnitTest	{
	void TestObjects();
}
