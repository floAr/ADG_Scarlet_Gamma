#include "WorldMessages.hpp"
#include "core/World.hpp"
#include <jofilelib.hpp>

namespace Network {

	void HandleWorldMessage( Core::World* _world, void* _data, size_t _size )
	{
		assert(_size > sizeof(MsgWorldHeader));

		switch(((MsgWorldHeader*)_data)->purpose)
		{
		case MsgWorld::LOAD:
			_world->Load( Jo::Files::MemFile((uint8_t*)_data + sizeof(MsgWorldHeader), _size - sizeof(MsgWorldHeader)) );
			break;
		}
	}

} // namespace Network