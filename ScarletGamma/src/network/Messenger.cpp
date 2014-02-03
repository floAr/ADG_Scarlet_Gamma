#include "Messenger.hpp"
#include "Core/World.hpp"
#include <iostream>
#include "Message.hpp"
#include "WorldMessages.hpp"
#include "MapMessages.hpp"
#include "ObjectMessages.hpp"
#include "CombatMessages.hpp"
#include "Game.hpp"
#include "ChatMessages.hpp"
#include "ActionMessages.hpp"

namespace Network {

	static const int BUFFER_SIZE = 10485760; // 10 MB
	static Messenger* g_msgInstance;


	Messenger::Messenger()
	{
		m_listener = new sf::TcpListener();
		if( m_listener->listen( 42961 ) != sf::Socket::Done )
			std::cout << "[Messenger] TcpListener cannot listen on port 42961";

		// Events are polled
		m_listener->setBlocking(false);

		m_buffer = malloc( BUFFER_SIZE );
	}

	Messenger::Messenger( const sf::IpAddress& _server ) :
		m_listener(nullptr)
	{
		// Establish the connection
		sf::TcpSocket* serverConnection = new sf::TcpSocket();
		if( serverConnection->connect( _server, 42961 ) != sf::Socket::Done )
		{
			std::cout << "[Messenger] Connection to " << _server << ':' << 42961 << " not possible";
			delete serverConnection;
			throw std::string("Cannot connect to server.");
		} else {
			m_buffer = malloc( BUFFER_SIZE );

			m_sockets.push_back(serverConnection);
		}
	}

	Messenger::~Messenger()
	{
		for( size_t i=0; i<m_sockets.size(); ++i )
		{
			delete m_sockets[i];
		}
		delete m_listener;
		free( m_buffer );
	}

	void Messenger::Initialize( const sf::IpAddress* _server )
	{
		if( _server == nullptr )
			g_msgInstance = new Messenger();
		else if( *_server == sf::IpAddress() )
			throw std::string("Invalid IP address.");
		else
			g_msgInstance = new Messenger( *_server );
	}

	void Messenger::Close()
	{
		delete g_msgInstance;
	}

	void Messenger::Poll( bool _blocking )
	{
		if( g_msgInstance->m_listener ) // This is the server
		{
			// Look for new clients
			sf::TcpSocket* newClient = new sf::TcpSocket();
			if( g_msgInstance->m_listener->accept(*newClient) == sf::Socket::Done)
			{
				// Add this client and sync (sent world...)
				g_msgInstance->AddClient( newClient );
			} // Ignore errors - there is not always a new client	
		}

		// Both server and client can receive any message
		for( size_t i=0; i<g_msgInstance->m_sockets.size(); ++i )
		{
			g_msgInstance->m_sockets[i]->setBlocking( _blocking );
			sf::Packet packet;
			if( _blocking )
			{
				// Handle exactly one message in blocking mode
				if( g_msgInstance->m_sockets[i]->receive( packet ) == sf::Socket::Done )
					g_msgInstance->HandleMessage(packet, g_msgInstance->m_sockets[i]);
			} else {
				while( g_msgInstance->m_sockets[i]->receive( packet ) == sf::Socket::Done )
					g_msgInstance->HandleMessage(packet, g_msgInstance->m_sockets[i]);
			}
		}
	}

	void Messenger::Send( void* _data, size_t _size, sf::TcpSocket* _to )
	{
		if( g_msgInstance )
		{
			sf::Packet packet;
			packet.append(_data, _size);
			if( _to )
				_to->send( packet );
			else for( size_t i=0; i<g_msgInstance->m_sockets.size(); ++i )
			{
				g_msgInstance->m_sockets[i]->send( packet );
			}
		}
	}

	void Messenger::AddClient( sf::TcpSocket* _newClient )
	{
		m_sockets.push_back( _newClient );
		// Sent whole world (in its latest state)
		MsgLoadWorld( g_Game->GetWorld(), _newClient ).Send();
	}


	void Messenger::HandleMessage( sf::Packet& _packet, sf::TcpSocket* _from )
	{
		const uint8_t* buffer = reinterpret_cast<const uint8_t*>(_packet.getData());
		const MessageHeader* header = reinterpret_cast<const MessageHeader*>(_packet.getData());
		size_t size = _packet.getDataSize() - sizeof(MessageHeader);
		size_t read = sizeof(MessageHeader);	// Deprecated but still here for testing

		// Get sender ID
		uint8_t id = std::distance(std::find(m_sockets.begin(), m_sockets.end(), _from), m_sockets.begin());

		switch(header->target)
		{
		case Target::WORLD:
			read += HandleWorldMessage(g_Game->GetWorld(), buffer + sizeof(MessageHeader), size);
			break;
		case Target::MAP:
			read += HandleMapMessage(g_Game->GetWorld()->GetMap(static_cast<Core::MapID>(header->targetID)),
				buffer + sizeof(MessageHeader), size);
			break;
		case Target::OBJECT:
			read += HandleObjectMessage(g_Game->GetWorld()->GetObject(static_cast<Core::ObjectID>(header->targetID)),
				buffer + sizeof(MessageHeader), size);
			break;
		case Target::CHAT:
			read += HandleChatMessage( buffer + sizeof(MessageHeader), size );
			break;
		case Target::ACTION:
			if ( IsServer() )
				read += HandleActionMessage( static_cast<Core::ActionID>(header->targetID), buffer + sizeof(MessageHeader), size, id );
			else
				read += HandleActionMessage( static_cast<Core::ActionID>(header->targetID), buffer + sizeof(MessageHeader), size, 0 );
			break;
		case Target::COMBAT:
			if ( IsServer() )
				read += HandleCombatMessage( buffer + sizeof(MessageHeader), size, id );
			else
				read += HandleCombatMessage( buffer + sizeof(MessageHeader), size, 0 );
			break;

			break;
		}

		assert(_packet.getDataSize() == read);

		if( IsServer() && (header->target != Target::ACTION) )
		{
			// Forward message to all other clients
			for( size_t i=0; i<g_msgInstance->m_sockets.size(); ++i )
			{
				if( g_msgInstance->m_sockets[i] != _from )
					g_msgInstance->m_sockets[i]->send( _packet );
			}
		}
	}

	bool Messenger::IsServer()
	{
		return g_msgInstance->m_listener != nullptr;
	}

	sf::TcpSocket* Messenger::GetSocket(uint8_t _index)
	{
		if (g_msgInstance)
			return g_msgInstance->m_sockets.at(_index);
		else
			return 0;
	}

} // namespace Network