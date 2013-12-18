#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include "MapMessages.hpp"
#include <jofilelib.hpp>

namespace Network {

	enum struct WorldMsgType: uint8_t
	{
		LOAD,
		ADD_MAP,
		ADD_OBJECT,
		REMOVE_MAP,
		REMOVE_OBJECT
	};

	/// \brief A base class for all world related messages.
	class WorldMsg
	{
	public:
		/// \brief Writes the headers and the specific data to a packet and
		///		sends it.
		void Send();

	private:
		WorldMsgType m_purpose;
		sf::TcpSocket* m_client;

	protected:
		WorldMsg( WorldMsgType _purpose, sf::TcpSocket* _client ) : m_purpose(_purpose), m_client(_client)	{}

		/// \brief Write data of a specific message to the packet.
		virtual void WriteData( Jo::Files::MemFile& _output ) const = 0;
	};

	/// \brief Suppress sending new messages, e.g. if a received message is
	///		evaluated the set-methods should not send the same things back.
	///	\details Just create an instance of the MaskWorldMessage type inside
	///		a scope to block new messages for the whole scope.
	///	
	///		The MaskWorldMessage automatically locks MapMessages too.
	struct MaskWorldMessage
	{
		MaskWorldMessage();
		~MaskWorldMessage();
	private:
		MaskMapMessage objMessageLock;
	};

	/// \brief Handle a message with target world.
	/// \details The function is called after the MessageHeader header is
	///		decoded. So data starts with MsgWorldHeader.
	///	\return Number of bytes which were read: could be smaller than _size.
	size_t HandleWorldMessage( Core::World* _world, uint8_t* _data, size_t _size );

	/// \brief Send the whole world to a client.
	/// \details Since a world is only send once on connection the specific
	///		client must be given (no broadcast).
	class MsgLoadWorld: public WorldMsg
	{
	public:
		///	\param [in] _world The world to be send.
		///	\param [in] _client The one new client.
		MsgLoadWorld( const Core::World* _world, sf::TcpSocket* _client );

		/// \see HandleWorldMessage
		static void Receive( Core::World* _world, Jo::Files::MemFile& _input );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::World* m_world;
	};
	//void SendLoadWorld( const Core::World* _world, sf::TcpSocket* _client );

	/// \brief Send a whole new map to all clients
	class MsgAddMap: public WorldMsg
	{
	public:
		/// \param [in] _map The map which should be sent and was not existent before.
		MsgAddMap( const Core::MapID _map );

		/// \see HandleWorldMessage
		static void Receive( Core::World* _world, Jo::Files::MemFile& _input );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::MapID m_map;
	};

	/// \brief Send a new object to all clients.
	class MsgAddObject: public WorldMsg
	{
	public:
		/// \param [in] _object The new object.
		MsgAddObject( const Core::ObjectID _object );

		/// \see HandleWorldMessage
		static void Receive( Core::World* _world, Jo::Files::MemFile& _input );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::ObjectID m_object;
	};

	/// \brief Send a remove request for a map to all clients.
	class MsgRemoveMap: public WorldMsg
	{
	public:
		MsgRemoveMap( const Core::MapID _map );

		/// \see HandleWorldMessage
		static void Receive( Core::World* _world, Jo::Files::MemFile& _input );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::MapID m_map;
	};
	//void SendRemoveMap( const Core::MapID _map );

	/// \brief Send a remove request for an object to all clients.
	class MsgRemoveObject: public WorldMsg
	{
	public:
		/// \param [in] _object The new object.
		MsgRemoveObject( const Core::ObjectID _object );

		/// \see HandleWorldMessage
		static void Receive( Core::World* _world, Jo::Files::MemFile& _input );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::ObjectID m_object;
	};
	//void SendRemoveObject( const Core::ObjectID _object );
} // namespace Network