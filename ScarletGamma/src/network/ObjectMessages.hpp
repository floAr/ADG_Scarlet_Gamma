#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include <jofilelib.hpp>

namespace Network {

	enum struct ObjectMsgType: uint8_t
	{
		SET_PROPERTY,		///< Add or change a property
		REMOVE_PROPERTY		///< Delete a property from an object
	};

	/// \brief A base class for all object related messages.
	class ObjectMsg
	{
	public:
		/// \brief Writes the headers and the specific data to a packet and
		///		sends it.
		void Send();

	private:
		ObjectMsgType m_purpose;
		const Core::ObjectID m_object;

	protected:
		ObjectMsg( ObjectMsgType _purpose, const Core::ObjectID _object ) : m_purpose(_purpose), m_object(_object)	{}

		/// \brief Write data of a specific message to the packet.
		virtual void WriteData( Jo::Files::MemFile& _output ) const = 0;
	};

	/// \brief Suppress sending new messages, e.g. if a received message is
	///		evaluated the set-methods should not send the same things back.
	///	\details Just create an instance of the MaskObjectMessage type inside
	///		a scope to block new messages for the whole scope.
	struct MaskObjectMessage
	{
		MaskObjectMessage();
		~MaskObjectMessage();
	};

	/// \brief Handle a message with target Object.
	/// \details The function is called after the MessageHeader header is
	///		decoded. So data starts with MsgObjectHeader.
	///	\return Number of bytes which were read: could be smaller than _size.
	size_t HandleObjectMessage( Core::Object* _object, const uint8_t* _data, size_t _size );

	/// \brief Send this message if a property is added or changed.
	class MsgPropertyChanged: public ObjectMsg
	{
	public:
		MsgPropertyChanged( const Core::ObjectID _object, const Core::Property* _property );

		/// \see HandleObjectMessage
		static size_t Receive( Core::Object* _object, const uint8_t* _data, size_t _size );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::Property* m_property;
	};

	/// \brief Send this message if a property is removed.
	class MsgRemoveProperty: public ObjectMsg
	{
	public:
		MsgRemoveProperty( const Core::ObjectID _object, const Core::Property* _property );

		/// \see HandleObjectMessage
		static size_t Receive( Core::Object* _object, const uint8_t* _data, size_t _size );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::Property* m_property;
	};
	//void SendRemoveProperty( const Core::ObjectID _object, const Core::Property* _property );
} // namespace Network