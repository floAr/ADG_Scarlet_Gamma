#pragma once

#include "Prerequisites.hpp"
#include <SFML/Network.hpp>
#include <cstdint>

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
		/// \param [in] _server An address of the server or nullptr
		///		address to initialize the server.
		///	\throws std::string
		static void Initialize( const sf::IpAddress* _server );

		/// \brief Release all resources and connections.
		static void Close();

		/// \brief Register some World/Player/... which can receive messages.
		static void RegisterTarget(/**/);

		/// \brief Look for new packets or clients and handle them.
		/// \param [in] _blocking Block current thread until a message is
		///		received.
		static void Poll( bool _blocking );

		/// \brief Send to server or broadcast an information.
		/// \details If this Messenger is the server the message is
		///		broadcasted per default.
		///	\param [in] _data Raw data to be send. Must begin with MessageHeader
		///	\param [in] _size Size of the data in bytes.
		///	\param [in] _to If nullptr send broadcasts to all connections. For
		///		clients that means they send the message to the server.
		///		In case it is another socket the message is send to one client
		///		only (relevant for server only).
		static void Send( const void* _data, size_t _size, sf::TcpSocket* _to = nullptr );

		/// \brief Returns if this application is the server.
		static bool IsServer();

		/// \brief Returns a socket by index.
		static sf::TcpSocket* GetSocket(uint8_t _index);
	private:
		sf::TcpListener* m_listener;			///< Server side listener to find new clients.
		std::vector<sf::TcpSocket*> m_sockets;	///< An array of connected servers/clients.
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
		///	\param [in] _from SocketID from whom the message was received in
		///		case to do proper answering.
		void HandleMessage( sf::Packet& _packet, unsigned _from );
	};

} // namespace Network