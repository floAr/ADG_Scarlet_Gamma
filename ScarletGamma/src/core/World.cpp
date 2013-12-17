#include "World.hpp"
#include "Object.hpp"
#include "Map.hpp"

using namespace std;

namespace Core {

	World::World()
	{
		// In an empty world the first possible ids are, well, the first available ids
		m_nextFreeMapID = 0;
		m_nextFreeObjectID = 0;
	}

	Map* World::GetMap(MapID _id)
	{
		return &m_maps.at(_id);
	}

	Object* World::GetObject(ObjectID _id)
	{
		return &m_objects.at(_id);
	}

	MapID World::NewMap( const std::string& _name, unsigned _sizeX, unsigned _sizeY )
	{
		m_maps.insert(std::make_pair<MapID,Map>(MapID(m_nextFreeMapID+0), Map(m_nextFreeMapID, _name, _sizeX, _sizeY, this)));
		++m_nextFreeMapID;
		return m_nextFreeMapID-1;
	}


	Core::ObjectID World::NewObject( const std::string& _sprite )
	{
		m_objects.insert(std::make_pair<ObjectID,Object>(ObjectID(m_nextFreeObjectID+0), Object(m_nextFreeObjectID, _sprite)));
		++m_nextFreeObjectID;
		return m_nextFreeObjectID-1;
	}


	void World::Load( Jo::Files::IFile& _file )
	{
		// Clear old stuff
		m_maps.clear();
		m_objects.clear();
		m_nextFreeMapID = 0;
		m_nextFreeObjectID = 0;

		// The save game is a top level file which contains maps and objects
		// serialized.
		const Jo::Files::MetaFileWrapper saveGame(_file);

		// There should be an array of objects
		const Jo::Files::MetaFileWrapper::Node* child;
		if( saveGame.RootNode.HasChild(string("Objects"), &child) )
		{
			// OK child should be an array of objects.
			for( unsigned i=0; i<child->Size(); ++i )
			{
				// Deserialize -> update maximum used ObjectID
				Object newObj((*child)[i]);
				m_nextFreeObjectID = max(m_nextFreeObjectID, newObj.ID());
				m_objects.insert(std::make_pair<ObjectID,Object>( newObj.ID(), std::move(newObj) ) );
			}
		} else {
			throw std::exception("Map file corrupted: cannot find the objects");
		}

		// Do the same for the maps
		if( saveGame.RootNode.HasChild(string("Maps"), &child) )
		{
			// OK child should be an array of objects.
			for( unsigned i=0; i<child->Size(); ++i )
			{
				// Deserialize -> update maximum used ObjectID
				Map newMap((*child)[i], this);
				m_nextFreeMapID = max(m_nextFreeMapID, newMap.ID());
				m_maps.insert(std::make_pair<MapID,Map>( newMap.ID(), std::move(newMap) ) );
			}
		} else {
			throw std::exception("Map file corrupted: cannot find the maps");
		}

		// The nextFree...ID is now the maximum used id. The increment yields
		// the first really unused id.
		++m_nextFreeObjectID;
		++m_nextFreeMapID;
	}

	void World::Save( Jo::Files::IFile& _file )
	{
		Jo::Files::MetaFileWrapper saveGame;
		// Serialize objects
		auto& objects = saveGame.RootNode.Add(string("Objects"), Jo::Files::MetaFileWrapper::ElementType::NODE, m_objects.size());
		int i=0;
		for( auto it=m_objects.begin(); it!=m_objects.end(); ++it, ++i )
			it->second.Serialize(objects[i]);

		// Serialize maps
		auto& maps = saveGame.RootNode.Add(string("Maps"), Jo::Files::MetaFileWrapper::ElementType::NODE, m_maps.size());
		i=0;
		for( auto it=m_maps.begin(); it!=m_maps.end(); ++it, ++i )
			it->second.Serialize(maps[i]);

		// Write a new file
		saveGame.Write(_file, Jo::Files::Format::JSON);
	}

} // namespace Core

















































