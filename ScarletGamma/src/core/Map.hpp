#pragma once

#include "Prerequisites.hpp"
#include <jofilelib.hpp>

namespace Core {

	/// \brief One map is the part of the world. Objects can switch between
	///		maps.
	///	\details Maps do only contain lists with object ids. The real objects
	///		are stored on a higher level (World).
	///		
	///		The order in the lists of the map are interpreted as layer. The
	///		smallest index 0 is the "floor". Object with an higher index are
	///		considered to overdraw the elements below.
	class Map
	{
	public:
		/// \brief C++11 Move construction.
		Map( Map&& _map );

		~Map();

		/// \brief Get the unique id of this map.
		MapID ID() const { return m_id; }

		/// \brief Extend the 2D map on one or more sides.
		/// \details The old object coordinates remain valid. So after expansion
		///		in negative direction negative coordinates are possible.
		/// \param [in] _nx Number in [0,oo] how much columns should be added
		///		in negative x direction.
		/// \param [in] _px Number in [0,oo] how much columns should be added
		///		in positive x direction.
		/// \param [in] _ny Number in [0,oo] how much rows should be added
		///		in negative y direction.
		/// \param [in] _py Number in [0,oo] how much rows should be added
		///		in positive y direction.
		void Extend(unsigned _nx, unsigned _px, unsigned _ny, unsigned _py);

		/// \brief Return a list of all objects at a given position.
		/// \details For positions outside the map this returns an empty list.
		///	\param [in] _x 2D position x component. The position is no array
		///		index. It is in [Left(),Right()].
		///	\param [in] _y 2D position y component. The position is no array
		///		index. It is in [Top(),Bottom()].
		const ObjectList& GetObjectsAt(int _x, int _y) const;
		ObjectList& GetObjectsAt(int _x, int _y);

		/// \brief Tests if there are no objects with collision in the
		///		grid cell.
		///	\param [in] _position 2D tile-position.
		bool IsFree(const sf::Vector2i& _position) const;

		/// \brief Add a new object between or on top of the existing objects.
		/// \details If the map position is not inside the current map the map
		///		will be extended.
		///		This method will send a network sync message.
		/// \param [in] _layer Where to insert the object in the render order?
		///		0 is the ground and objects with negative values are not drawn.
		void Add(ObjectID _object, int _x, int _y, int _layer);

		/// \brief Remove an object from the map. The object itself is not deleted!
		/// \details The attributes for position and layer are removed from
		///		the object.
		///		This method will send a network sync message.
		///	\param [in] _object ID of the object which should be deleted. If the
		///		object is not on the map or has an invalid position state an
		///		assertion will fail.
		void Remove(ObjectID _object);

		/// \brief Number for cells in x-direction
		int Width() const	{ return m_maxX-m_minX+1; }
		/// \brief Smallest coordinate in x-direction
		int Left() const	{ return m_minX; }
		/// \brief Largest coordinate in x-direction
		int Right() const	{ return m_maxX; }
		/// \brief Number for cells in y-direction
		int Height() const	{ return m_maxY-m_minY+1; }
		/// \brief Smallest coordinate in y-direction
		int Top() const		{ return m_minY; }
		/// \brief Largest coordinate in y-direction
		int Bottom() const	{ return m_maxY; }

		/// \brief Update all active objects on the map (mostly path finding
		///		and movements of players and NPCs.
		///	\param [in] _dt	Delta time since last frame in seconds.
		void Update(float _dt);

		/// \brief Write the content of this map to a meta-file.
		/// \details Serialization contains the map ids.
		/// \param [inout] _node A node with ElementType::UNKNOWN which can
		///		be changed and expanded by serialize.
		void Serialize( Jo::Files::MetaFileWrapper::Node& _node ) const;

		/// \brief A* path search from one tile to another.
		/// \return A stack of points which must be reached one after one
		///		where each next point can be reached by linear interpolation.
		///		The next point to reach is always on top of the stack (back
		///		of vector).
		///		
		///		The vector is empty if no way exists.
		std::vector<sf::Vector2i> FindPath( sf::Vector2i _start, sf::Vector2i _goal ) const;

		/// \brief Returns one of the 8 neighbors which is the first to be
		///		visited to go to the goal.
		sf::Vector2i FindNextOnPath( sf::Vector2i _start, sf::Vector2i _goal ) const;

		/// \brief return the highest layer of all contained objects.
		int GetMaxLayer() const { return m_maxLayer; }

		/// \brief Set the object attributes and reinsert object in tile grid
		///		if necessary.
		///	\details This also send a message to the network to autosync the
		///		games.
		///	\param [inout] _object The object whose position is to be set.
		///	\param [in] _position Floating position where the integer part
		///		gives the tile position. The position must be within the grid.
		void SetObjectPosition( Object* _object, const sf::Vector2f& _position );

		/// \brief Make sure an object is in the cell where its position says it is.
		void ResetGridPosition( ObjectID _object, const sf::Vector2i& _oldCell, const sf::Vector2i& _newCell );

		const std::string& GetName() const		{ return m_name; }
	private:
		std::string m_name;

		ObjectList* m_mapArray;	///< 2D cell array.
		int m_minX, m_maxX;		///< Size (max-min+1) and position in x direction
		int m_minY, m_maxY;		///< Size (max-min+1) and position in y direction
		int m_maxLayer;			///< Highest used number in the layering required for render order
		ObjectList* m_activeObjects;	///< All objects in the mapArray which needs to be updated.

		World* m_parentWorld;	///< World reference to have direct object access.

		MapID m_id;				///< Unique identification of this map

		// No copy construction and assignments of maps allowed
		Map( const Map& _map );
		Map& operator=(const Map& _map);


		/// Use private constructors such that only the world can create maps.
		friend class Core::World;

		/// \brief Create a map with an initial size (can be changed later).
		/// \param [in] _name A name which is players choice.
		/// \param [in] _sizeX Initial map size. The tile coordinates go
		///		from 0 to _size-1.
		/// \param [in] _sizeY Initial map size. The tile coordinates go
		///		from 0 to _size-1.
		///	\param [in] _world The world stores the real objects. The map
		///		requires a world reference to check their properties.
		Map(MapID _id, const std::string& _name, unsigned _sizeX, unsigned _sizeY, World* _world);

		/// \brief Deserialize an map.
		/// \param [in] _node A serialized map node.
		///	\param [in] _world The world stores the real objects. The map
		///		requires a world reference to check their properties.
		Map(const Jo::Files::MetaFileWrapper::Node& _node, World* _world);

		/// \brief AI helper method to find the next target for an object.
		/// \details A target is a grid cell which can be reached by a linear
		///		walk.
		///		
		///		The target is drawn from the "Path" property (first element).
		///		The list is updated if the current position is equal to the
		///		first point in the path list.
		sf::Vector2f FindNextTarget(Object* _object) const;

		/// \brief AI helper method to test if an objects position is equal
		///		to the target position.
		///	\param [in] _object The object to test.
		///	\param [in] _position The current objects position (to avoid
		///		computational overhead).
		bool HasReachedTarget(Object* _object, const sf::Vector2f& _position) const;
	};

} // namespace Core