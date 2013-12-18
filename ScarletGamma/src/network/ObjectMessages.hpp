#pragma once

#include <cstdint>
#include "Prerequisites.hpp"

namespace Network {

	enum struct MsgObject: uint8_t
	{
		SET_PROPERTY,		///< Add or change a property
		REMOVE_PROPERTY		///< Delete a property from an object
	};

	/// \brief A header for world-targeted messages.
	struct MsgObjectHeader
	{
		MsgObject purpose;

		MsgObjectHeader( MsgObject _purpose ) : purpose(_purpose)	{}
	};

	/// \brief Handle a message with target Object.
	/// \details The function is called after the MessageHeader header is
	///		decoded. So data starts with MsgObjectHeader.
	///	\return Number of bytes which were read: could be smaller than _size.
	size_t HandleObjectMessage( Core::Object* _object, void* _data, size_t _size );

	/// \brief Suppress sending new messages, e.g. if a received message is
	///		evaluated the set-methods should not send the same things back.
	///	\details Just create an instance of the MaskObjectMessage type inside
	///		a scope to block new messages for the whole scope.
	struct MaskObjectMessage
	{
		MaskObjectMessage();
		~MaskObjectMessage();
	};

	/// \brief Send this message if a property is added or changed.
	void SendPropertyChanged( const Core::ObjectID _object, const Core::Property* _property );

	/// \brief Send this message if a property is removed.
	void SendRemoveProperty( const Core::ObjectID _object, const Core::Property* _property );
} // namespace Network