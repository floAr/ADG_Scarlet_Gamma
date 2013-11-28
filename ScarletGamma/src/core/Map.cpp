#include "Map.hpp"
#include "Object.hpp"

namespace Core {

	Map::Map( unsigned _sizeX, unsigned _sizeY, World* _world ) :
		m_minX(0),
		m_maxX(_sizeX-1),
		m_minY(0),
		m_maxY(_sizeY-1),
		m_parentWorld(_world)
	{
		m_mapArray = new ObjectList[_sizeX * _sizeY];
	}

	Map::~Map()
	{
		delete[] m_mapArray;
	}

	void Map::Extend( unsigned _nx, unsigned _px, unsigned _ny, unsigned _py )
	{
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
		// TODO
		return true;
	}

	void Map::Add(ObjectID _object, int _x, int _y, int _layer)
	{
		auto list = GetObjectsAt(_x, _y);
		// TODO: layering
		list.Add(_object);
	}

} // namespace Core