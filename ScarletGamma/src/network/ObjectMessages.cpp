#include "Messenger.hpp"
#include "Message.hpp"
#include "ObjectMessages.hpp"
#include "core/World.hpp"
#include <jofilelib.hpp>

namespace Network {

	/// \brief This variable "locks" sending messages to avoid a recursion
	///		during receive.
	static int g_MaskNewObjectMessages = 0;

	MaskObjectMessage::MaskObjectMessage() { ++g_MaskNewObjectMessages; }
	MaskObjectMessage::~MaskObjectMessage() { --g_MaskNewObjectMessages; }

	size_t HandleObjectMessage( Core::Object* _object, void* _data, size_t _size )
	{
		assert(_size > sizeof(MsgObjectHeader));
		MaskObjectMessage messageLock;

		// Map memory for deserialization
		Jo::Files::MemFile file((uint8_t*)_data + sizeof(MsgObjectHeader), _size - sizeof(MsgObjectHeader));

		switch(((MsgObjectHeader*)_data)->purpose)
		{
		case MsgObject::SET_PROPERTY: {
			// Deserialize and than remove the old and add the deserialized one.
			Core::Property prop(_object->ID(), Jo::Files::MetaFileWrapper(file).RootNode );
			_object->Remove( prop.Name() );
			_object->Add( prop );
			} break;
		case MsgObject::REMOVE_PROPERTY: {
			// Deserialize to get the name
			Core::Property prop(_object->ID(), Jo::Files::MetaFileWrapper(file).RootNode );
			_object->Remove( prop.Name() );
			} break;
		}

		return size_t(file.GetCursor() + sizeof(MsgObjectHeader));
	}

	void SendPropertyChanged( const Core::ObjectID _object, const Core::Property* _property )
	{
		if( g_MaskNewObjectMessages == 0 )
		{
			// Write headers
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::OBJECT, _object ), sizeof(MessageHeader) );
			data.Write( &MsgObjectHeader( MsgObject::SET_PROPERTY ), sizeof(MsgObjectHeader) );
			// Serialize the property
			Jo::Files::MetaFileWrapper propertyData;
			_property->Serialize( propertyData.RootNode );
			propertyData.Write( data, Jo::Files::Format::SRAW );

			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}


	void SendRemoveProperty( const Core::ObjectID _object, const Core::Property* _property )
	{
		if( g_MaskNewObjectMessages == 0 )
		{
			// Do mostly the same as in SendPropertyChanged() (eaysier).
			// Only the purpose of the message differs
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::OBJECT, _object ), sizeof(MessageHeader) );
			data.Write( &MsgObjectHeader( MsgObject::REMOVE_PROPERTY ), sizeof(MsgObjectHeader) );
			Jo::Files::MetaFileWrapper propertyData;
			_property->Serialize( propertyData.RootNode );
			propertyData.Write( data, Jo::Files::Format::SRAW );
			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}


} // namespace Network