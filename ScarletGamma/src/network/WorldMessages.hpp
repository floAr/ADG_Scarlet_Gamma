#pragma once

#include <cstdint>
#include "Prerequisites.hpp"

namespace Network {

	enum struct MsgWorld: uint8_t
	{
		LOAD,
		ADD_MAP,
		ADD_OBJECT,
		REMOVE_MAP,
		REMOVE_OBJECT
	};

	/// \brief A header for world-targeted messages.
	struct MsgWorldHeader
	{
		MsgWorld purpose;

		MsgWorldHeader( MsgWorld _purpose ) : purpose(_purpose)	{}
	};

	/// \brief Handle a message with target world.
	/// \details The function is called after the MessageHeader header is
	///		decoded. So data starts with MsgWorldHeader.
	void HandleWorldMessage( Core::World* _world, void* _data, size_t _size );

} // namespace Network