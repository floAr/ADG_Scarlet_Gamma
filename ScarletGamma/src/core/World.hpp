#pragma once

#include <unordered_map>
#include <jofilelib.hpp>
#include "Prerequisites.hpp"
#include "Map.hpp"
#include "Object.hpp"

namespace Core {

	class World
	{
	public:
		/// \brief Start with an empty world.
		World();

		/// \brief Get one of the maps by a handle which is equal for server
		///		and clients.
		Map* GetMap(MapID _id);

		/// \brief Get an object by its unique _id. The id is equal for server
		///		and clients.
		Object* GetObject(ObjectID _id);

		/// \brief Creates a new map and derives a new id for it.
		/// \param [in] _name A name for the new map.
		/// \param [in] _sizeX Initial width of the map. It can be resized
		///		later.
		///	\param [in] _sizeY Initial height of the map. It can be resized
		///		later.
		MapID NewMap(const std::string& _name, unsigned _sizeX, unsigned _sizeY);

		/// \brief Creates a new object and derives a new id for it.
		/// \details The object is not assigned to a world or an inventory, ...
		/// \param [in] Every object must be render able
		ObjectID NewObject( const std::string& _sprite );

		/// \brief Loads all maps and objects of a world from a save-game.
		void Load( Jo::Files::IFile& _file );

		/// \brief Stores all maps and objects to a save game.
		/// TODO: how are players handled?
		void Save( Jo::Files::IFile& _file );
	private:
		/// \brief All real existing objects.
		std::unordered_map<ObjectID, Object> m_objects;
		ObjectID m_nextFreeObjectID;	///< The next unused object handle

		/// \brief All loaded maps.
		std::unordered_map<MapID, Map> m_maps;
		MapID m_nextFreeMapID;		///< A handle which is not used up to now
	};
} // namespace Core