#pragma once

#include <cstdint>
#include "Prerequisites.hpp"
#include "SFML\System\String.hpp"
#include "SFML\Graphics\Color.hpp"

namespace Network {

	/// \brief The one and only message for the chat target.
	class ChatMsg
	{
	public:
		/// \brief Writes the headers and the specific data to a packet and
		///		sends it.
		void Send();

		//ChatMsg( const sf::String& _msg ) : m_msg(_msg)		{}
		ChatMsg( const std::string& _msg, const sf::Color _color, bool _forMasterOnly = false ) :
			m_msg(_msg),
			m_color(_color),
			m_forward(!_forMasterOnly)
		{}

		const std::string& Text() const	{ return m_msg; }
		const sf::Color& Color() const	{ return m_color; }
	private:
		std::string m_msg;
		sf::Color m_color;
		bool m_forward;

	protected:
	};

	/// \brief Handle a message with target Chat.
	/// \details The function is called after the MessageHeader header is
	///		decoded. So data starts with ChatMsg.
	///	\return Number of bytes which were read: could be smaller than _size.
	size_t HandleChatMessage( const uint8_t* _data, size_t _size );

} // namespace Network