#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include "ObjectMessages.hpp"
#include <jofilelib.hpp>

namespace Network {

	enum struct MapMsgType: uint8_t
	{
		SET_OBJECT_POSITION,		///< Change position of an object which is in the map
		INSERT_OBJECT,				///< Insert existing object to the map
		REMOVE_OBJECT				///< Remove object reference from map
	};

	/// \brief A header for world-targeted messages.
	class MapMsg
	{
	public:
		/// \brief Writes the headers and the specific data to a packet and
		///		sends it.
		void Send();

	private:
		MapMsgType m_purpose;
		const Core::MapID m_map;

	protected:
		MapMsg( MapMsgType _purpose, const Core::MapID _map ) : m_purpose(_purpose), m_map(_map)	{}

		/// \brief Write data of a specific message to the packet.
		virtual void WriteData( Jo::Files::MemFile& _output ) const = 0;
	};

	/// \brief Suppress sending new messages, e.g. if a received message is
	///		evaluated the set-methods should not send the same things back.
	///	\details Just create an instance of the MaskMapMessage type inside
	///		a scope to block new messages for the whole scope.
	///	
	///		The MaskMapMessage automatically locks ObjectMessages too.
	struct MaskMapMessage
	{
		MaskMapMessage();
		~MaskMapMessage();
	private:
		MaskObjectMessage objMessageLock;
	};

	/// \brief Handle a message with target Map.
	/// \details The function is called after the MessageHeader header is
	///		decoded. So data starts with MsgMapHeader.
	///	\return Number of bytes which were read: could be smaller than _size.
	size_t HandleMapMessage( Core::Map* _map, uint8_t* _data, size_t _size );

	/// \brief Send this message if a property is added or changed.
	class MsgObjectPositionChanged: public MapMsg
	{
	public:
		MsgObjectPositionChanged( const Core::MapID _map, const Core::ObjectID _object, const sf::Vector2f& _position );

		/// \see HandleMapMessage
		static size_t Receive( Core::Map* _map, uint8_t* _data, size_t _size );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::ObjectID m_object;
		const sf::Vector2f m_position;
	};

	/// \brief Synchronize insertion of objects into maps
	class MsgInsertObjectToMap: public MapMsg
	{
	public:
		MsgInsertObjectToMap( const Core::MapID _map, const Core::ObjectID _object, int _x, int _y, int _layer );

		/// \see HandleMapMessage
		static size_t Receive( Core::Map* _map, uint8_t* _data, size_t _size );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::ObjectID m_object;
		const int m_x, m_y, m_layer;
	};
	//void SendInsertObjectToMap( const Core::MapID _map, const Core::ObjectID _object, int _x, int _y, int _layer );

	/// \brief Synchronize removal of objects from maps
	class MsgRemoveObjectFromMap: public MapMsg
	{
	public:
		MsgRemoveObjectFromMap( const Core::MapID _map, const Core::ObjectID _object );

		/// \see HandleMapMessage
		static size_t Receive( Core::Map* _map, uint8_t* _data, size_t _size );
	protected:
		virtual void WriteData( Jo::Files::MemFile& _output ) const override;
	private:
		const Core::ObjectID m_object;
	};
	//void SendRemoveObjectFromMap( const Core::MapID _map, const Core::ObjectID _object );
} // namespace Network