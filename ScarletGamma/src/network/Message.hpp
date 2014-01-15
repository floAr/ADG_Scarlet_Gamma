#pragma once

#include <cstdint>

namespace Network {

	/// \brief What is the type of the receiver?
	enum struct Target: uint8_t
	{
		WORLD,		///< Something which changes the whole world
		MAP,		///< Something which changes a specific map
		OBJECT,		///< Something which changes a specific map
		CHAT,		///< Text for the chat
		MASTER,		///< Action message: request to the game master
        PLAYER      ///< Action message: request to a player
	};

	/// \brief Opening of all packets to decide what to do and what data was
	///		sent.
	struct MessageHeader
	{
		Target target;		///< Type of target
		uint64_t targetID;	///< ID if there are multiple targets of the same type.

		MessageHeader( Target _target, uint64_t _id ) : target(_target), targetID(_id)	{}
	};

} // namespace Network