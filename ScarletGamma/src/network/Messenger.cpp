#include "Messenger.hpp"
#include "Core/World.hpp"
#include <iostream>
#include "Message.hpp"
#include "WorldMessages.hpp"
#include "MapMessages.hpp"
#include "ObjectMessages.hpp"
#include "Game.hpp"

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
		m_buffer = malloc( BUFFER_SIZE );

		// Establish the connection
		sf::TcpSocket* serverConnection = new sf::TcpSocket();
		if( serverConnection->connect( _server, 42961 ) != sf::Socket::Done )
		{
			std::cout << "[Messenger] Connection to " << _server << ':' << 42961 << " not possible";
		} else
			m_sockets.push_back(serverConnection);
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

	void Messenger::Initialize( const sf::IpAddress& _server )
	{
		if( _server == sf::IpAddress() )
			g_msgInstance = new Messenger();
		else
			g_msgInstance = new Messenger( _server );
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
			size_t receivedBytes;
			if( g_msgInstance->m_sockets[i]->receive(g_msgInstance->m_buffer, BUFFER_SIZE, receivedBytes) == sf::Socket::Done )
				g_msgInstance->HandleMessage(receivedBytes);
		}
	}

	void Messenger::Send( void* _data, size_t _size )
	{
		if( g_msgInstance )
		{
			for( size_t i=0; i<g_msgInstance->m_sockets.size(); ++i )
			{
				g_msgInstance->m_sockets[i]->send( _data, _size );
			}
		}
	}

	void Messenger::AddClient( sf::TcpSocket* _newClient )
	{
		m_sockets.push_back( _newClient );
		// Sent whole world (in its latest state)
		MsgLoadWorld( g_Game->GetWorld(), _newClient ).Send();
	}


	void Messenger::HandleMessage( size_t _size )
	{
		uint8_t* buffer = (uint8_t*)m_buffer;
		while( _size > sizeof(MessageHeader) )
		{
			MessageHeader* header = reinterpret_cast<MessageHeader*>(buffer);
			size_t read = sizeof(MessageHeader);
			switch(header->target)
			{
			case Target::WORLD:
				read += HandleWorldMessage(g_Game->GetWorld(), buffer + sizeof(MessageHeader), _size);
				break;
			case Target::MAP:
				read += HandleMapMessage(g_Game->GetWorld()->GetMap(static_cast<Core::MapID>(header->targetID)),
					buffer + sizeof(MessageHeader), _size);
				break;
			case Target::OBJECT:
				read += HandleObjectMessage(g_Game->GetWorld()->GetObject(static_cast<Core::ObjectID>(header->targetID)),
					buffer + sizeof(MessageHeader), _size);
				break;
			}

			buffer += read;
			_size -= read;
		}
	}

} // namespace Network