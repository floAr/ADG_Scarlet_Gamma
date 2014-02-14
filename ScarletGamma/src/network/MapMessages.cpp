#include "Messenger.hpp"
#include "Message.hpp"
#include "MapMessages.hpp"
#include "core/World.hpp"
#include "Game.hpp"
#include <jofilelib.hpp>

namespace Network {

	/// \brief This variable "locks" sending messages to avoid a recursion
	///		during receive.
	static int g_MaskNewMapMessages = 0;

	MaskMapMessage::MaskMapMessage() { ++g_MaskNewMapMessages; }
	MaskMapMessage::~MaskMapMessage() { --g_MaskNewMapMessages; }

	size_t HandleMapMessage( Core::Map* _map, const uint8_t* _data, size_t _size )
	{
		assert(_size > sizeof(MapMsgType));
		MaskMapMessage messageLock;

		const MapMsgType* header = reinterpret_cast<const MapMsgType*>(_data);
		size_t readSize = sizeof(MapMsgType);

		switch(*header)
		{
		case MapMsgType::SET_OBJECT_POSITION:
			readSize += MsgObjectPositionChanged::Receive(_map, _data + readSize, _size - readSize);
			break;
		case MapMsgType::INSERT_OBJECT:
			readSize += MsgInsertObjectToMap::Receive(_map, _data + readSize, _size - readSize);
			break;
		case MapMsgType::REMOVE_OBJECT:
			readSize += MsgRemoveObjectFromMap::Receive(_map, _data + readSize, _size - readSize);
			break;
		}

		return readSize;
	}

	void MapMsg::Send()
	{
		if( g_MaskNewMapMessages == 0 )
		{
			// Write headers
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::MAP, m_map, true ), sizeof(MessageHeader) );
			data.Write( &m_purpose, sizeof(MapMsgType) );
			// Write data
			WriteData( data );

			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}


	// ********************************************************************* //
	MsgObjectPositionChanged::MsgObjectPositionChanged( const Core::MapID _map, const Core::ObjectID _object, const sf::Vector2f& _position ) :
		MapMsg(MapMsgType::SET_OBJECT_POSITION, _map),
		m_object(_object),
		m_position(_position)
	{
	}

	void MsgObjectPositionChanged::WriteData( Jo::Files::MemFile& _output ) const
	{
		// Write data
		_output.Write( &m_object, sizeof(Core::ObjectID) );
		_output.Write( &m_position, sizeof(sf::Vector2f) );
	}

	size_t MsgObjectPositionChanged::Receive( Core::Map* _map, const uint8_t* _data, size_t _size )
	{
		const Core::ObjectID* object = reinterpret_cast<const Core::ObjectID*>(_data);
		const sf::Vector2f* position = reinterpret_cast<const sf::Vector2f*>(_data + sizeof(Core::ObjectID));

		_map->SetObjectPosition( g_Game->GetWorld()->GetObject(*object), *position );

		return sizeof(Core::ObjectID) + sizeof(sf::Vector2f);
	}


	

	// ********************************************************************* //
	MsgInsertObjectToMap::MsgInsertObjectToMap( const Core::MapID _map, const Core::ObjectID _object, int _x, int _y, int _layer ) :
		MapMsg( MapMsgType::INSERT_OBJECT, _map ),
		m_object( _object ),
		m_x(_x), m_y(_y), m_layer(_layer)
	{
	}

	void MsgInsertObjectToMap::WriteData( Jo::Files::MemFile& _output ) const
	{
		_output.Write( &m_object, sizeof(Core::ObjectID) );
		_output.Write( &m_x, sizeof(int) );
		_output.Write( &m_y, sizeof(int) );
		_output.Write( &m_layer, sizeof(int) );
	}

	size_t MsgInsertObjectToMap::Receive( Core::Map* _map, const uint8_t* _data, size_t _size )
	{
		size_t readSize = 0;
		const Core::ObjectID* object = reinterpret_cast<const Core::ObjectID*>(_data);	readSize += sizeof(Core::ObjectID);
		const int* x = reinterpret_cast<const int*>(_data + readSize);			readSize += sizeof(int);
		const int* y = reinterpret_cast<const int*>(_data + readSize);			readSize += sizeof(int);
		const int* layer = reinterpret_cast<const int*>(_data + readSize);		readSize += sizeof(int);
		_map->Add( *object, *x, *y, *layer );
		return readSize;
	}

	

	// ********************************************************************* //
	MsgRemoveObjectFromMap::MsgRemoveObjectFromMap( const Core::MapID _map, const Core::ObjectID _object ) :
		MapMsg( MapMsgType::REMOVE_OBJECT, _map ),
		m_object( _object )
	{
	}

	void MsgRemoveObjectFromMap::WriteData( Jo::Files::MemFile& _output ) const
	{
		_output.Write( &m_object, sizeof(Core::ObjectID) );
	}

	size_t MsgRemoveObjectFromMap::Receive( Core::Map* _map, const uint8_t* _data, size_t _size )
	{
		const Core::ObjectID* object = reinterpret_cast<const Core::ObjectID*>(_data);
		_map->Remove( *object );
		return sizeof(Core::ObjectID);
	}


} // namespace Network