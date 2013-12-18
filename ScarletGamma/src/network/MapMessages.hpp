#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include "ObjectMessages.hpp"

namespace Network {

	enum struct MsgMap: uint8_t
	{
		SET_OBJECT_POSITION,		///< Change position of an object which is in the map
	};

	/// \brief A header for world-targeted messages.
	struct MsgMapHeader
	{
		MsgMap purpose;

		MsgMapHeader( MsgMap _purpose ) : purpose(_purpose)	{}
	};

	/// \brief Handle a message with target Object.
	/// \details The function is called after the MessageHeader header is
	///		decoded. So data starts with MsgObjectHeader.
	///	\return Number of bytes which were read: could be smaller than _size.
	size_t HandleMapMessage( Core::Map* _map, void* _data, size_t _size );

	/// \brief Suppress sending new messages, e.g. if a received message is
	///		evaluated the set-methods should not send the same things back.
	///	\details Just create an instance of the MaskMapMessage type inside
	///		a scope to block new messages for the whole scope.
	///	
	///		The MaskMapMessage automatically locks ObjectMessages too.
	struct MaskMapMessage
	{
		MaskMapMessage();
		~MaskMapMessage();
	private:
		MaskObjectMessage objMessageLock;
	};

	/// \brief Send this message if a property is added or changed.
	void SendObjectPositionChanged( const Core::MapID _map, const Core::ObjectID _object, const sf::Vector2f& _position );

} // namespace Network