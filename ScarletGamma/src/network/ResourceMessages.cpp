#include "Messenger.hpp"
#include "Message.hpp"
#include "ResourceMessages.hpp"
#include "core/World.hpp"
#include <jofilelib.hpp>
#include "Game.hpp"
#include "utils/Content.hpp"

using namespace Jo::Files;

namespace Network {

	size_t HandleResourceMessage( const uint8_t* _data, size_t _size )
	{
		// Read message type (Request or Send)
		uint8_t type = *_data;
		_data += 1;

		size_t size = 1;

		// What was the message?
		if( type == 0 )
		{
			// Unpack the resource list
			std::vector<std::string> list;
			// 16 Bit count
			size_t num = ((uint16_t*)_data)[0];
			_data += 2;	size += 2;
			for( size_t i = 0; i < num; ++i )
			{
				// 16 Bit string length
				size_t length = ((uint16_t*)_data)[0];
				_data += 2; size += 2;
				// Create string of known length
				std::string filename((char*)_data, length);
				_data += length; size += length;
				list.push_back( filename );
			}

			Content::Instance()->Synchronize( list );
		} else if( type == 1 )
		{
			// Read the filename
			// 16 Bit string length
			size_t length = ((uint16_t*)_data)[0];
			_data += 2; size += 2;
			// Create string of known length
			std::string filename((char*)_data, length);
			_data += length; size += length;
			filename = "media/" + filename;

			// Request -> look if we have the searched file
			if( Jo::Files::Utils::Exists( filename ) )
			{
				// Send resource back
				MsgResource(filename).Send();
			} else assert(false);
		} else {
			// Receive

			// Read the filename
			// 16 Bit string length
			size_t length = ((uint16_t*)_data)[0];
			_data += 2; size += 2;
			// Create string of known length
			std::string filename((char*)_data, length);
			_data += length; size += length;

			// 32 Bit file size
			uint32_t fileSize = ((uint32_t*)_data)[0];
			_data += 4; size += 4 + fileSize;
			if( !Jo::Files::Utils::Exists( filename ) )	// Accidentally sent twice
			{
				HDDFile file( filename, HDDFile::CREATE_FILE );
				file.Write(_data, fileSize);
			}
		}

		return size;
	}

	void MsgRequestResource::Send()
	{
		Jo::Files::MemFile data;

		// The packet contains the type (Request/Send) and the file name
		uint16_t length = m_filename.size();
		uint8_t type = 1;
		data.Write( &MessageHeader( Target::RESOURCES, 0, false ), sizeof(MessageHeader) );
		data.Write( &type, sizeof(uint8_t) );
		data.Write( &length, sizeof(uint16_t) );
		data.Write( m_filename.c_str(), length );

		Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
	}


	void MsgResourceList::Send()
	{
		Jo::Files::MemFile data;

		// The packet contains the type (Request/Send) and lots of file names
		uint8_t type = 0;
		data.Write( &MessageHeader( Target::RESOURCES, 0, false ), sizeof(MessageHeader) );
		data.Write( &type, sizeof(uint8_t) );

		uint16_t num = m_files.size();
		data.Write( &num, sizeof(uint16_t) );
		for( auto it = m_files.begin(); it != m_files.end(); ++it )
		{
			uint16_t length = it->size();
			data.Write( &length, sizeof(uint16_t) );
			data.Write( it->c_str(), length );
		}

		Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
	}


	void MsgResource::Send()
	{
		Jo::Files::MemFile data;

		// The packet contains the type (Request/Send), the file name and the
		// file data
		uint16_t length = m_filename.size();
		uint8_t type = 2;
		data.Write( &MessageHeader( Target::RESOURCES, 0, false ), sizeof(MessageHeader) );
		data.Write( &type, sizeof(uint8_t) );
		data.Write( &length, sizeof(uint16_t) );
		data.Write( m_filename.c_str(), length );
		// Copy file
		HDDFile file( m_filename );
		uint32_t size = (uint32_t)file.GetSize();
		data.Write( &size, sizeof(uint32_t) );
		file.Read( file.GetSize(), data.Reserve( file.GetSize() ) );

		Messenger::Send( data.GetBuffer(), (size_t)data.GetSize() );
	}

} // namespace Network