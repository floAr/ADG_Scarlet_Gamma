#pragma once

#include "Prerequisites.hpp"
#include <SFML/Network.hpp>

namespace Network {

	/// \brief A central unit to send messages and receive that from the other
	///		gamers.
	///	\details This is a singleton which also could be used inside one client
	///		to send messages from the bottom to world ...
	///		
	///		The Messenger always uses the same port: 42961
	///		and all connections are TCP
	class Messenger
	{
	public:
		/// \brief Connect to a game master-server or create the server.
		/// \param [in] _server An address of the server or the empty (invalid)
		///		address to initialize the server.
		static void Initialize( const sf::IpAddress& _server );

		/// \brief Release all resources and connections.
		static void Close();

		/// \brief Register some World/Player/... which can receive messages.
		static void RegisterTarget(/**/);

		/// \brief Look for new packets or clients and handle them.
		static void Poll();
	private:
		sf::TcpListener* m_listener;			///< Server side listener to find new clients.
		std::vector<sf::TcpSocket*> m_sockets;	///< An array of connected servers/clients.
		Core::World* m_world;					///< The world which is played and synchronized.
		void* m_buffer;							///< A large buffer to receive any message.

		/// \brief Server constructor
		Messenger();

		/// \brief Client constructor
		Messenger( const sf::IpAddress& _server );

		~Messenger();

		/// \brief Add a new socked to the list and sent all data to the user
		///		to bring him up to date.
		void AddClient( sf::TcpSocket* _newClient );

		/// \brief Analyze the headers and deliver the message to the
		///		appropriate target handler.
		///	\details The received data must be stored in m_buffer.
		void HandleMessage( size_t _size );
	};

} // namespace Network