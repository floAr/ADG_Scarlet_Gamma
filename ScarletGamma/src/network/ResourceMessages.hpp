#pragma once

#include <cstdint>
#include <unordered_set>
#include "Prerequisites.hpp"
#include "SFML\System\String.hpp"
#include "SFML\Graphics\Color.hpp"

namespace Network {

	/// \brief Send this if you wanna have a file from someone.
	class MsgRequestResource
	{
	public:
		/// \brief Writes the headers and the specific data to a packet and
		///		sends it.
		void Send();

		MsgRequestResource( const std::string& _filename ) :
			m_filename(_filename)
		{}

	private:
		std::string m_filename;
	};

	// Send a list of all known files
	class MsgResourceList
	{
	public:
		/// \brief Writes the headers and the specific data to a packet and
		///		sends it.
		void Send();

		MsgResourceList( const std::unordered_set<std::string>& _files ) :
			m_files(_files)
		{}

	private:
		const std::unordered_set<std::string>& m_files;
	};

	/// \brief Send as request answer: contains the resource
	class MsgResource
	{
	public:
		/// \brief Writes the headers and the specific data to a packet and
		///		sends it.
		void Send();

		MsgResource( const std::string& _filename ) :
			m_filename(_filename)
		{}

	private:
		std::string m_filename;
	};

	/// \brief Handle a message with target Resources.
	/// \details The function is called after the MessageHeader header is
	///		decoded.
	///	\return Number of bytes which were read: could be smaller than _size.
	size_t HandleResourceMessage( const uint8_t* _data, size_t _size );

} // namespace Network