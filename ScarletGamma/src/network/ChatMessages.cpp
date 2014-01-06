#include "Messenger.hpp"
#include "Message.hpp"
#include "ChatMessages.hpp"
#include "core/World.hpp"
#include <jofilelib.hpp>
#include "Game.hpp"

namespace Network {

	size_t HandleChatMessage( const uint8_t* _data, size_t _size )
	{
		sf::Color color = *(sf::Color*)_data;
		_data += sizeof(sf::Color);

		// 16 Bit string length
		size_t length = ((uint16_t*)_data)[0];
		_data += 2;

		// Create string of known length
		std::string text((char*)_data, length);

		ChatMsg chatMessage(text, color);

		// If server receives a chat message it must be broadcasted back.
		if( Messenger::IsServer() )
		{
			chatMessage.Send();
		} else
			// Client - store chat message
			g_Game->AppendToChatLog( chatMessage );

		return sizeof(sf::Color) + 2 + length;
	}

	void ChatMsg::Send()
	{
		// Write string length
		Jo::Files::MemFile data;

		uint16_t length = m_msg.size();
		data.Write( &MessageHeader( Target::CHAT, 0 ), sizeof(MessageHeader) );
		data.Write( &m_color, sizeof(sf::Color) );
		data.Write( &length, sizeof(uint16_t) );
		data.Write( m_msg.c_str(), length );

		Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );

		// If a client sends the message it will get it back through the
		// broadcast. In case of the server this is not the case.
		if(Messenger::IsServer())
		{
			g_Game->AppendToChatLog( *this );
		}
	}

	

} // namespace Network