/// \file Prerequisites.h
/// Contains forward declarations for all / most of the classes.

#pragma once

//------------------------------------------------------------------------------
// GLOBAL NAMESPACE
class Game;

//------------------------------------------------------------------------------
// UTILS
namespace Utils
{
    class Random;
    class Falloff;
}

namespace Exception {
    class NotEvaluateable;
    class NoObjectList;
    class NoSuchProperty;
    class InvalidFormula;
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

    /// \brief General definition of an action handle.
    /// \details This is an index to one of the actions in the ActionPool.
    typedef unsigned int ActionID;

    /// \brief A map handle inside the world.
    typedef unsigned int MapID;

	/// \brief Player IDs are used for the right system and network communication.
	///		The id 0 is always the game master.
	typedef unsigned int PlayerID;
}

//------------------------------------------------------------------------------
// EVENTS
namespace Events
{
    class EventHandler;
    class InputHandler;
}

//------------------------------------------------------------------------------
// NETWORK
namespace Network
{
    class Messenger;
}

//------------------------------------------------------------------------------
// ACTIONS
namespace Actions
{
    class Action;
    class ActionPool;
}

//------------------------------------------------------------------------------
// STATES
namespace States
{
    class StateMachine;
    class GameState;
    class PromptState;
}

//------------------------------------------------------------------------------
// GRAPHICS
namespace Graphics
{
    class TileRenderer;
}

//------------------------------------------------------------------------------
// INTERFACES
namespace Interfaces
{
	class PropertyPanel;
	class ObjectPanel;
	struct DragContent;
}


//------------------------------------------------------------------------------
// MATHEMATICS
#include "sfutils/Vectors.hpp"

//------------------------------------------------------------------------------
// TEST
namespace UnitTest	{
    void TestObjects();
}



//------------------------------------------------------------------------------
// sf
namespace sf {
    class TcpSocket;
} // namespace sf