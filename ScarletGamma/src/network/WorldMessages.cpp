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

	size_t HandleWorldMessage( Core::World* _world, const uint8_t* _data, size_t _size )
	{
		assert(_size > sizeof(WorldMsgType));
		MaskWorldMessage messageLock;

		Jo::Files::MemFile file(_data + sizeof(WorldMsgType), _size - sizeof(WorldMsgType));
		switch(*((WorldMsgType*)_data))
		{
		case WorldMsgType::LOAD:
			MsgLoadWorld::Receive( _world, file );
			break;
		case WorldMsgType::ADD_MAP:
			MsgAddMap::Receive( _world, file );
			break;
		case WorldMsgType::ADD_OBJECT:
			MsgAddObject::Receive( _world, file );
			break;
		case WorldMsgType::REMOVE_MAP:
			MsgRemoveMap::Receive( _world, file );
			break;
		case WorldMsgType::REMOVE_OBJECT:
			MsgRemoveObject::Receive( _world, file );
			break;
		}

		return size_t(sizeof(WorldMsgType) + file.GetCursor());
	}

	void WorldMsg::Send()
	{
		if( g_MaskNewWorldMessages == 0 )
		{
			// Write headers
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::WORLD, 0 ), sizeof(MessageHeader) );
			data.Write( &m_purpose, sizeof(WorldMsgType) );
			// Write data
			WriteData( data );

			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize(), m_client );
		}
	}


	// ********************************************************************* //
	MsgLoadWorld::MsgLoadWorld( const Core::World* _world, sf::TcpSocket* _client ) :
		WorldMsg(WorldMsgType::LOAD, _client),
		m_world(_world)
	{
	}

	void MsgLoadWorld::WriteData( Jo::Files::MemFile& _output ) const
	{
		// Write data
		m_world->Save( _output );
	}

	void MsgLoadWorld::Receive( Core::World* _world, Jo::Files::MemFile& _input )
	{
		_world->Load( _input );
	}
	
	/*void SendLoadWorld( const Core::World* _world, sf::TcpSocket* _client )
	{
		if( g_MaskNewWorldMessages == 0 )
		{
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::WORLD, 0 ), sizeof(MessageHeader) );
			data.Write( &MsgWorldHeader( MsgWorld::LOAD ), sizeof(MsgWorldHeader) );
			_world->Save( data );
			_client->send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}*/

	// ********************************************************************* //
	MsgAddMap::MsgAddMap( const Core::MapID _map ) :
		WorldMsg(WorldMsgType::ADD_MAP, nullptr),
		m_map(_map)
	{
	}

	void MsgAddMap::WriteData( Jo::Files::MemFile& _output ) const
	{
		// Serialize the map
		Jo::Files::MetaFileWrapper mapData;
		g_Game->GetWorld()->GetMap(m_map)->Serialize( mapData.RootNode );
		mapData.Write( _output, Jo::Files::Format::SRAW );
	}

	void MsgAddMap::Receive( Core::World* _world, Jo::Files::MemFile& _input )
	{
		_world->NewMap( Jo::Files::MetaFileWrapper(_input).RootNode );
	}

	/*void SendAddMap( const Core::MapID _map )
	{
		if( g_MaskNewWorldMessages == 0 )
		{
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::WORLD, 0 ), sizeof(MessageHeader) );
			data.Write( &MsgWorldHeader( MsgWorld::ADD_MAP ), sizeof(MsgWorldHeader) );
			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}*/

	// ********************************************************************* //
	MsgAddObject::MsgAddObject( const Core::World* _world, const Core::ObjectID _object ) :
		WorldMsg(WorldMsgType::ADD_OBJECT, nullptr),
		m_object(_object)
	{
		// Currently synchronizing other worlds is not supported.
		if( g_MaskNewWorldMessages == 0 )
			assert( _world == g_Game->GetWorld() );
	}

	void MsgAddObject::WriteData( Jo::Files::MemFile& _output ) const
	{
		// Serialize the object
		Jo::Files::MetaFileWrapper objectData;
		g_Game->GetWorld()->GetObject(m_object)->Serialize( objectData.RootNode );
		objectData.Write( _output, Jo::Files::Format::SRAW );
	}

	void MsgAddObject::Receive( Core::World* _world, Jo::Files::MemFile& _input )
	{
		_world->NewObject( Jo::Files::MetaFileWrapper(_input).RootNode );
	}

	/*void SendAddObject( const Core::ObjectID _object )
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
	}*/

	// ********************************************************************* //
	MsgRemoveMap::MsgRemoveMap( const Core::MapID _map ) :
		WorldMsg(WorldMsgType::REMOVE_MAP, nullptr),
		m_map(_map)
	{
	}

	void MsgRemoveMap::WriteData( Jo::Files::MemFile& _output ) const
	{
		// Remove request are easy: just send the ID.
		_output.Write( &m_map, sizeof(Core::MapID) );
	}

	void MsgRemoveMap::Receive( Core::World* _world, Jo::Files::MemFile& _input )
	{
		Core::MapID id;
		_input.Read( sizeof(Core::MapID), &id );
		_world->RemoveMap(id);
	}

/*	void SendRemoveMap( const Core::MapID _map )
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
	}*/

	// ********************************************************************* //
	MsgRemoveObject::MsgRemoveObject( const Core::ObjectID _object ) :
		WorldMsg(WorldMsgType::REMOVE_OBJECT, nullptr),
		m_object(_object)
	{
	}

	void MsgRemoveObject::WriteData( Jo::Files::MemFile& _output ) const
	{
		// Remove request are easy: just send the ID.
		_output.Write( &m_object, sizeof(Core::ObjectID) );
	}

	void MsgRemoveObject::Receive( Core::World* _world, Jo::Files::MemFile& _input )
	{
		Core::ObjectID id;
		_input.Read( sizeof(Core::ObjectID), &id );
		_world->RemoveObject(id);
	}
	/*void SendRemoveObject( const Core::ObjectID _object )
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
	}*/

} // namespace Network