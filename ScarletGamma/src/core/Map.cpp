#include "Map.hpp"
#include "Object.hpp"
#include "World.hpp"
#include "utils/OrFibHeap.h"

#include <algorithm>
#include <unordered_map>

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

	bool Map::IsFree( const sf::Vector2i& _position ) const
	{
		// Test each object in this cell if it has obstacle property
		auto list = GetObjectsAt(_position.x, _position.y);
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
		object->SetPosition(float(_x), float(_y));
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



	struct SearchNode {
		SearchNode* predecessor;
		sf::Vector2i cell;
		float costs;
		OrE::ADT::FibHeapNode<SearchNode*>* entry;	///< nullptr if on closed list otherwise a reference to the entry on the open list

		SearchNode(SearchNode* _predecessor, float _costs, const sf::Vector2i& _cell) :
			predecessor(_predecessor),
			costs(_costs),
			cell(_cell),
			entry(nullptr)
		{}
	};

	std::vector<sf::Vector2i> Map::FindPath( sf::Vector2i _start, sf::Vector2i _goal )
	{
		// Persistent memory of all visited nodes.
		std::list<SearchNode> visited;
		// A* requires a heap with a decrease-key operation. The STL variants
		// do not support this operation.
		OrE::ADT::HeapT<SearchNode*> openList;
		visited.push_back( SearchNode(nullptr, 0.0f, _start) );
		visited.front().entry = openList.Insert( &visited.front(), 0.0f );
		// References into the visited list to search for a position
		std::unordered_map<int64_t, SearchNode*> visitedAccess;
		visitedAccess.insert(make_pair(_start.x | (int64_t(_start.y) << 32), &visited.front()));

		while(openList.GetNumElements()>0)
		{
			// The object is in the list so it can be deleted from stack immediately
			auto minEntry = openList.Min();
			SearchNode* node = minEntry->Object;
			openList.Delete(minEntry);
			// Stop if node is the goal
			if( node->cell == _goal )
			{
				// Build a path vector (stack)
				std::vector<sf::Vector2i> path;
				while(node) {
					path.push_back(node->cell);
					node = node->predecessor;
				}
				return path;
			}
			// Otherwise search in 8 neighborhood for new nodes.
			node->entry = nullptr;
			for(int y=-1; y<=1; ++y) {
				for(int x=-1; x<=1; ++x) {
					sf::Vector2i successorPos = node->cell+sf::Vector2i(x,y);
					int64_t hash = successorPos.x | (int64_t(successorPos.y) << 32);
					auto next = visitedAccess.find(hash);
					if( next != visitedAccess.end() )
					{
						// Already on index -> Update or not?
						if( next->second->entry &&
							next->second->costs > node->costs+sfUtils::Length(_goal-successorPos) )
						{
							next->second->costs = node->costs+sfUtils::Length(_goal-successorPos);
							openList.ChangeKey(next->second->entry, next->second->costs);
						}
					} else {
						// Is this neighbor a possible field?
						if(IsFree(successorPos)) continue;
						// Insert
						visited.push_back( SearchNode(node, node->costs+sfUtils::Length(_goal-successorPos), successorPos) );
						visited.back().entry = openList.Insert(&visited.back(), visited.back().costs);
						visitedAccess.insert(make_pair(hash, &visited.back()));
					}
				}
			}
		}

		// Nothing found:
		return std::vector<sf::Vector2i>();
	}

	sf::Vector2i Map::FindNextOnPath( sf::Vector2i _start, sf::Vector2i _goal )
	{
		// This function can be optimized by avoiding the build of the whole path.
		return FindPath(_start, _goal).back();
	}

} // namespace Core