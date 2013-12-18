#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include "MapMessages.hpp"

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

	/// \brief Suppress sending new messages, e.g. if a received message is
	///		evaluated the set-methods should not send the same things back.
	///	\details Just create an instance of the MaskWorldMessage type inside
	///		a scope to block new messages for the whole scope.
	///	
	///		The MaskWorldMessage automatically locks MapMessages too.
	struct MaskWorldMessage
	{
		MaskWorldMessage();
		~MaskWorldMessage();
	private:
		MaskMapMessage objMessageLock;
	};

	/// \brief Handle a message with target world.
	/// \details The function is called after the MessageHeader header is
	///		decoded. So data starts with MsgWorldHeader.
	///	\return Number of bytes which were read: could be smaller than _size.
	size_t HandleWorldMessage( Core::World* _world, void* _data, size_t _size );

	/// \brief Send the whole world to a client.
	/// \details Since a world is only send once on connection the specific
	///		client must be given (no broadcast).
	///	\param [in] _world The world to be send.
	///	\param [in] _client The one new client.
	void SendLoadWorld( const Core::World* _world, sf::TcpSocket* _client );

	/// \brief Send a whole new map to all clients
	/// \param [in] _map The map which should be sent and was not existent before.
	void SendAddMap( const Core::MapID _map );

	/// \brief Send a new object to all clients.
	/// \param [in] _object The new object.
	void SendAddObject( const Core::ObjectID _object );

	/// \brief Send a remove request for a map to all clients.
	void SendRemoveMap( const Core::MapID _map );

	/// \brief Send a remove request for an object to all clients.
	void SendRemoveObject( const Core::ObjectID _object );
} // namespace Network