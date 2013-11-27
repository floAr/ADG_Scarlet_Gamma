#pragma once

#include "Prerequisites.hpp"

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
		/// \brief Create a map with an initial size (can be changed later).
		/// \param [in] _sizeX Initial map size. The tile coordinates go
		///		from 0 to _size-1.
		/// \param [in] _sizeY Initial map size. The tile coordinates go
		///		from 0 to _size-1.
		///	\param [in] _world The world stores the real objects. The map
		///		requires a world reference to check their properties.
		Map(unsigned _sizeX, unsigned _sizeY, World* _world);

		~Map();

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
		///	\param [in] _x 2D position x component.
		///	\param [in] _y 2D position y component.
		bool IsFree(int _x, int _y) const;

		/// \brief Add a new object between or on top of the existing objects.
		/// \param [in] _layer Where to insert the object in the list? The
		///		default -1 adds the element on top of the stack (at the end).
		void Add(ObjectID _object, int _x, int _y, int _layer = -1);

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
	private:
		ObjectList* m_mapArray;	///< 2D cell array.
		int m_minX, m_maxX;		///< Size (max-min+1) and position in x direction
		int m_minY, m_maxY;		///< Size (max-min+1) and position in y direction

		World* m_parentWorld;	///< World reference to have direct object access.

		// No copy construction and assignments of maps allowed
		Map( const Map& _map );
		Map& operator=(const Map& _map);
	};

} // namespace Core