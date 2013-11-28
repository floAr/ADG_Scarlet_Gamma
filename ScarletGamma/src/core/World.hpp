#pragma once

#include <unordered_map>
#include "Prerequisites.hpp"

namespace Core {

	class World
	{
	public:
		/// \brief Get one of the maps by a handle which is equal for server
		///		and clients.
		Map* GetMap(MapID _id);

		/// \brief Get an object by its unique _id. The id is equal for server
		///		and clients.
		Object* GetObject(ObjectID _id);
	private:
		/// \brief All real existing objects.
		std::unordered_map<ObjectID, Object*> m_objects;
	};
} // namespace Core