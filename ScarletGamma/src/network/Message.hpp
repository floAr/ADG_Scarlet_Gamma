#pragma once

#include <cstdint>

namespace Network {

	/// \brief What is the type of the receiver?
	enum struct Target: uint8_t
	{
		WORLD,		///< Something which changes the whole world
		MAP,		///< Something which changes a specific map
		OBJECT,		///< Something which changes a specific map
		ACTION,		///< Something which changes the state of an action
		COMBAT,		///< Something which changes the combat state
		CHAT,		///< Text for the chat
		RESOURCES	///< Resource synchronization
	};

	/// \brief Opening of all packets to decide what to do and what data was
	///		sent.
	struct MessageHeader
	{
		Target target;		///< Type of target
		uint64_t targetID;	///< ID if there are multiple targets of the same type.
		bool forward;		///< != 0 if the message should be broadcasted.

		MessageHeader( Target _target, uint64_t _id, bool _forward ) : target(_target), targetID(_id), forward(_forward)	{}
	};

} // namespace Network