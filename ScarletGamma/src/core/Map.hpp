#pragma once

#include "Prerequisites.hpp"

namespace Core {

	/// \brief One map is the part of the world. Objects can switch between
	///		maps.
	///	\details Maps do only contain lists with object ids. The real objects
	///		are stored on a higher level.
	class Map
	{
	public:
		/// \brief Create a map with an initial size (can be changed later).
		/// \param [in] _sizeX 
		/// \param [in] _sizeY
		Map(unsigned _sizeX, unsigned _sizeY);

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
		ObjectList GetObjectsAt();

		/// \brief Searches if there are no objects with collision in the
		///		grid cell.
		bool IsFree();
	private:
		ObjectList* m_mapArray;
	};

} // namespace Core