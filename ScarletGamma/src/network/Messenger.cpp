#include "Messenger.hpp"
#include "Core/World.hpp"
#include <iostream>
#include "Message.hpp"
#include "WorldMessages.hpp"

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

	void Messenger::Poll()
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
			size_t receivedBytes;
			if( g_msgInstance->m_sockets[i]->receive(g_msgInstance->m_buffer, BUFFER_SIZE, receivedBytes) )
				g_msgInstance->HandleMessage(receivedBytes);
		}
	}

	void Messenger::AddClient( sf::TcpSocket* _newClient )
	{
		_newClient->setBlocking( false );
		m_sockets.push_back( _newClient );
		// Sent whole world (in its latest state)
		Jo::Files::MemFile data;
		data.Write( &MessageHeader( Target::WORLD, 0 ), sizeof(MessageHeader) );
		data.Write( &MsgWorldHeader( MsgWorld::LOAD ), sizeof(MsgWorldHeader) );
		m_world->Save( data );
		_newClient->send( data.GetBuffer(), (size_t)data.GetSize() );
	}


	void Messenger::HandleMessage( size_t _size )
	{
		assert( _size > sizeof(MessageHeader) );
		switch(((MessageHeader*)m_buffer)->target)
		{
		case Target::WORLD:
			HandleWorldMessage(m_world, (uint8_t*)m_buffer + sizeof(MessageHeader), _size - sizeof(MessageHeader));
			break;
		}
	}

} // namespace Network