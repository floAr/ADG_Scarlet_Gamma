#include "Map.hpp"
#include "Object.hpp"
#include "World.hpp"

#include <algorithm>

using namespace std;

namespace Core {

	Map::Map( MapID _id, const std::string& _name, unsigned _sizeX, unsigned _sizeY, World* _world ) :
		m_name(_name),
		m_minX(0),
		m_maxX(_sizeX-1),
		m_minY(0),
		m_maxY(_sizeY-1),
		m_parentWorld(_world),
		m_id(_id)
	{
		m_mapArray = new ObjectList[_sizeX * _sizeY];
	}

	Map::Map( Map&& _map ) :
		m_name(_map.m_name),
		m_minX(_map.m_minX),
		m_maxX(_map.m_maxX),
		m_minY(_map.m_minY),
		m_maxY(_map.m_maxY),
		m_parentWorld(_map.m_parentWorld),
		m_id(_map.m_id)
	{
		// Move the only real resource
		m_mapArray = _map.m_mapArray;
		_map.m_mapArray = nullptr;
	}

	Map::Map(const Jo::Files::MetaFileWrapper::Node& _node)
	{
		m_name = _node.GetName();
		m_id = _node[string("ID")];
		m_minX = _node[string("MinX")];
		m_minY = _node[string("MinY")];
		m_maxX = _node[string("MaxX")];
		m_maxY = _node[string("MaxY")];
		m_mapArray = new ObjectList[Width() * Height()];
		auto& cells = _node[string("Cells")];
		for(int y=0; y<Height(); ++y)
			for(int x=0; x<Width(); ++x)
			{
				int index = x+y*Width();
				m_mapArray[index] = ObjectList(cells[index]);
			}
	}

	Map::~Map()
	{
		delete[] m_mapArray;
	}

	void Map::Extend( unsigned _nx, unsigned _px, unsigned _ny, unsigned _py )
	{
		if( _nx+_px+_ny+_py == 0 ) return;	// Nothing would happen

		// Allocate a new larger memory
		ObjectList* mapArray = new ObjectList[(Width()+_nx+_px) * (Height()+_ny+_py)];
		int newWidth = Width() + _nx + _ny;
		// Copy old part
		for( int y=0; y<Height(); ++y ) {
			for( int x=0; x<Width(); ++x ) {
				// Take the memory away from the old array (move much faster)
				mapArray[x+_nx+(y+_ny)*newWidth] = std::move(m_mapArray[x+y*Width()]);
			}
		}
		// The delete should delete not more than the memory block of the array
		// itself. The rest is moved to the new one.
		delete[] m_mapArray;

		m_mapArray = m_mapArray;
		m_minX -= _nx;
		m_minY -= _ny;
		m_maxX += _px;
		m_maxY += _py;
	}

	const ObjectList& Map::GetObjectsAt( int _x, int _y ) const
	{
		// Out of bounds -> empty world
		static ObjectList TheEmptyList;
		if( _x < m_minX || _x > m_maxX ) return TheEmptyList;
		if( _y < m_minY || _y > m_maxY ) return TheEmptyList;

		return m_mapArray[_x-m_minX + (_y-m_minY)*Width()];
	}

	ObjectList& Map::GetObjectsAt(int _x, int _y)
	{
		// Save standard get. The const_cast here is not wrong since the Add
		// method does not have a const qualifier.
		return const_cast<ObjectList&>( const_cast<const Map*>(this)->GetObjectsAt(_x, _y) );
	}

	bool Map::IsFree( int _x, int _y ) const
	{
		// Test each object in this cell if it has obstacle property
		auto list = GetObjectsAt(_x, _y);
		for( int i=0; i<list.Size(); ++i )
			if(m_parentWorld->GetObject(list[i])->HasProperty(string("Obstacle")))
				return false;
		return true;
	}

	void Map::Add(ObjectID _object, int _x, int _y, int _layer)
	{
		Extend(std::max(m_minX-_x,0), std::max(_x-m_maxX,0), std::max(m_minY-_y,0), std::max(_y-m_maxY,0));

		auto& list = GetObjectsAt(_x, _y);
		// TODO: layering
		list.Add(_object);

		// Set correct position for the object itself
		Object* object = m_parentWorld->GetObject(_object);
		object->SetPosition(_x, _y);
	}

	void Map::Serialize( Jo::Files::MetaFileWrapper::Node& _node )
	{
		_node.SetName(m_name);
		_node[string("ID")] = m_id;
		_node[string("MinX")] = m_minX;
		_node[string("MinY")] = m_minY;
		_node[string("MaxX")] = m_maxX;
		_node[string("MaxY")] = m_maxY;
		auto& cells = _node.Add(string("Cells"), Jo::Files::MetaFileWrapper::ElementType::NODE, Width()*Height() );
		for(int y=0; y<Height(); ++y)
			for(int x=0; x<Width(); ++x)
			{
				int index = x+y*Width();
				m_mapArray[index].Serialize(cells[index]);
			}
	}

} // namespace Core