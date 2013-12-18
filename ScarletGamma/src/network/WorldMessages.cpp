#include "Messenger.hpp"
#include "Message.hpp"
#include "WorldMessages.hpp"
#include "core/World.hpp"
#include "Game.hpp"
#include <jofilelib.hpp>

namespace Network {

	/// \brief This variable "locks" sending messages to avoid a recursion
	///		during receive.
	static int g_MaskNewWorldMessages = 0;

	MaskWorldMessage::MaskWorldMessage() { ++g_MaskNewWorldMessages; }
	MaskWorldMessage::~MaskWorldMessage() { --g_MaskNewWorldMessages; }

	size_t HandleWorldMessage( Core::World* _world, void* _data, size_t _size )
	{
		assert(_size > sizeof(MsgWorldHeader));
		MaskWorldMessage messageLock;

		Jo::Files::MemFile file((uint8_t*)_data + sizeof(MsgWorldHeader), _size - sizeof(MsgWorldHeader));
		switch(((MsgWorldHeader*)_data)->purpose)
		{
		case MsgWorld::LOAD:
			_world->Load( file );
			break;
		case MsgWorld::ADD_MAP:
			_world->NewMap( Jo::Files::MetaFileWrapper(file).RootNode );
			break;
		case MsgWorld::ADD_OBJECT:
			_world->NewObject( Jo::Files::MetaFileWrapper(file).RootNode );
			break;
		case MsgWorld::REMOVE_MAP: {
			Core::MapID id;
			file.Read( sizeof(Core::MapID), &id );
			_world->RemoveMap(id);
			} break;
		case MsgWorld::REMOVE_OBJECT: {
			Core::ObjectID id;
			file.Read( sizeof(Core::ObjectID), &id );
			_world->RemoveObject(id);
			} break;
		}

		return size_t(sizeof(MsgWorldHeader) + file.GetCursor());
	}

	void SendLoadWorld( const Core::World* _world, sf::TcpSocket* _client )
	{
		if( g_MaskNewWorldMessages == 0 )
		{
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::WORLD, 0 ), sizeof(MessageHeader) );
			data.Write( &MsgWorldHeader( MsgWorld::LOAD ), sizeof(MsgWorldHeader) );
			_world->Save( data );
			_client->send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}

	void SendAddMap( const Core::MapID _map )
	{
		if( g_MaskNewWorldMessages == 0 )
		{
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::WORLD, 0 ), sizeof(MessageHeader) );
			data.Write( &MsgWorldHeader( MsgWorld::ADD_MAP ), sizeof(MsgWorldHeader) );
			// Serialize the map
			Jo::Files::MetaFileWrapper mapData;
			g_Game->GetWorld()->GetMap(_map)->Serialize( mapData.RootNode );
			mapData.Write( data, Jo::Files::Format::SRAW );
			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}

	void SendAddObject( const Core::ObjectID _object )
	{
		if( g_MaskNewWorldMessages == 0 )
		{
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::WORLD, 0 ), sizeof(MessageHeader) );
			data.Write( &MsgWorldHeader( MsgWorld::ADD_OBJECT ), sizeof(MsgWorldHeader) );
			// Serialize the object
			Jo::Files::MetaFileWrapper objectData;
			g_Game->GetWorld()->GetObject(_object)->Serialize( objectData.RootNode );
			objectData.Write( data, Jo::Files::Format::SRAW );
			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}

	void SendRemoveMap( const Core::MapID _map )
	{
		if( g_MaskNewWorldMessages == 0 )
		{
			// Remove request are easy: just send the ID.
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::WORLD, 0 ), sizeof(MessageHeader) );
			data.Write( &MsgWorldHeader( MsgWorld::REMOVE_MAP ), sizeof(MsgWorldHeader) );
			data.Write( &_map, sizeof(Core::MapID) );
			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}

	void SendRemoveObject( const Core::ObjectID _object )
	{
		if( g_MaskNewWorldMessages == 0 )
		{
			// Remove request are easy: just send the ID.
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::WORLD, 0 ), sizeof(MessageHeader) );
			data.Write( &MsgWorldHeader( MsgWorld::REMOVE_OBJECT ), sizeof(MsgWorldHeader) );
			data.Write( &_object, sizeof(Core::ObjectID) );
			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}

} // namespace Network