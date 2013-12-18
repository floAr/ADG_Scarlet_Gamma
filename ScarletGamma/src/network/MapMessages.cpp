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

	size_t HandleMapMessage( Core::Map* _map, void* _data, size_t _size )
	{
		assert(_size > sizeof(MsgMapHeader));
		MaskMapMessage messageLock;

		MsgMapHeader* header = reinterpret_cast<MsgMapHeader*>(_data);
		Core::ObjectID* object = reinterpret_cast<Core::ObjectID*>((uint8_t*)_data + sizeof(MsgMapHeader));
		sf::Vector2f* position = reinterpret_cast<sf::Vector2f*>((uint8_t*)object + sizeof(Core::ObjectID));

		switch(header->purpose)
		{
		case MsgMap::SET_OBJECT_POSITION: {
			_map->SetObjectPosition( g_Game->GetWorld()->GetObject(*object), *position );
			} break;
		}

		return sizeof(MsgMapHeader) + sizeof(Core::ObjectID) + sizeof(sf::Vector2f);
	}

	void SendObjectPositionChanged( const Core::MapID _map, const Core::ObjectID _object, const sf::Vector2f& _position )
	{
		if( g_MaskNewMapMessages == 0 )
		{
			// Write headers
			Jo::Files::MemFile data;
			data.Write( &MessageHeader( Target::MAP, _map ), sizeof(MessageHeader) );
			data.Write( &MsgMapHeader( MsgMap::SET_OBJECT_POSITION ), sizeof(MsgMapHeader) );
			// Write data
			data.Write( &_object, sizeof(Core::ObjectID) );
			data.Write( &_position, sizeof(sf::Vector2f) );

			Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
		}
	}


} // namespace Network