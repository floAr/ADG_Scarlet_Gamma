#include "Messenger.hpp"
#include "Message.hpp"
#include "ObjectMessages.hpp"
#include "core/World.hpp"
#include <jofilelib.hpp>
#include <iostream>// TODO: remove

namespace Network {

	/// \brief This variable "locks" sending messages to avoid a recursion
	///		during receive.
	static int g_MaskNewObjectMessages = 0;

	MaskObjectMessage::MaskObjectMessage() { ++g_MaskNewObjectMessages; }
	MaskObjectMessage::~MaskObjectMessage() { --g_MaskNewObjectMessages; }

	size_t HandleObjectMessage( Core::Object* _object, const uint8_t* _data, size_t _size )
	{
		if( !_object ) throw std::string("[HandleObjectMessage] Target object does not exists.");
		assert(_size > sizeof(ObjectMsgType));
		MaskObjectMessage messageLock;

		const ObjectMsgType* header = reinterpret_cast<const ObjectMsgType*>(_data);
		size_t readSize = sizeof(ObjectMsgType);

		switch(*header)
		{
		case ObjectMsgType::SET_PROPERTY:
			readSize += MsgPropertyChanged::Receive(_object, _data + readSize, _size - readSize);
			break;
		case ObjectMsgType::REMOVE_PROPERTY:
			readSize += MsgRemoveProperty::Receive(_object, _data + readSize, _size - readSize);
			break;
		}

		return readSize;
	}

	void ObjectMsg::Send()
	{
		if( g_MaskNewObjectMessages == 0 )
		{
			// Write headers
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::OBJECT, m_object, true ), sizeof(MessageHeader) );
			data.Write( &m_purpose, sizeof(ObjectMsgType) );
			// Write data
			WriteData( data );

			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}


	// ********************************************************************* //
	MsgPropertyChanged::MsgPropertyChanged( const Core::ObjectID _object, const Core::Property* _property ) :
		ObjectMsg( ObjectMsgType::SET_PROPERTY, _object ),
		m_property( _property )
	{
	}

	void MsgPropertyChanged::WriteData( Jo::Files::MemFile& _output ) const
	{
		// Serialize the property
		Jo::Files::MetaFileWrapper propertyData;
		m_property->Serialize( propertyData.RootNode );
		propertyData.Write( _output, Jo::Files::Format::SRAW );
	}

	size_t MsgPropertyChanged::Receive( Core::Object* _object, const uint8_t* _data, size_t _size )
	{
		// Deserialize and than remove the old and add the deserialized one.
		Jo::Files::MemFile file(_data, _size);
		Core::Property prop(_object->ID(), Jo::Files::MetaFileWrapper(file).RootNode );
	//	std::cout << prop.Name() << ' ' << prop.Value() << ' ' << prop.GetObjects().Size() << '\n';
	//	for(int i=0; i<prop.GetObjects().Size(); ++i) std::cout << prop.GetObjects()[i] << '\n';
		_object->Add( prop );
		return (size_t)file.GetCursor();
	}

	


	// ********************************************************************* //
	MsgRemoveProperty::MsgRemoveProperty( const Core::ObjectID _object, const Core::Property* _property ) :
		ObjectMsg( ObjectMsgType::REMOVE_PROPERTY, _object ),
		m_property( _property )
	{
	}

	void MsgRemoveProperty::WriteData( Jo::Files::MemFile& _output ) const
	{
		// Serialize the property
		Jo::Files::MetaFileWrapper propertyData;
		m_property->Serialize( propertyData.RootNode );
		propertyData.Write( _output, Jo::Files::Format::SRAW );
	}

	size_t MsgRemoveProperty::Receive( Core::Object* _object, const uint8_t* _data, size_t _size )
	{
		// Deserialize to get the name
		Jo::Files::MemFile file(_data, _size);
		Core::Property prop(_object->ID(), Jo::Files::MetaFileWrapper(file).RootNode );
		_object->Remove( prop.Name() );

		return (size_t)file.GetCursor();
	}

	

} // namespace Network