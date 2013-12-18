#include "WorldMessages.hpp"
#include "core/World.hpp"
#include <jofilelib.hpp>

namespace Network {

	size_t HandleWorldMessage( Core::World* _world, void* _data, size_t _size )
	{
		assert(_size > sizeof(MsgWorldHeader));

		Jo::Files::MemFile file((uint8_t*)_data + sizeof(MsgWorldHeader), _size - sizeof(MsgWorldHeader));
		switch(((MsgWorldHeader*)_data)->purpose)
		{
		case MsgWorld::LOAD:
			_world->Load( file );
			break;
		}

		return size_t(sizeof(MsgWorldHeader) + file.GetCursor());
	}

} // namespace Network