#pragma once

#include "Prerequisites.hpp"

namespace Tools {

	/// \brief A class to draw on a map.
	class Brush
	{
	public:
		/// \brief Initialize an brush without parameters (painting would just
		///		do nothing)
		Brush();

		~Brush();

		enum Mode {
			ADD,		///< Just add the new object
			REPLACE,	///< If there is an object on the layer replace it
			DELETE		///< Delete all occurrences of object matching the name from _obj
		};

		/// \brief Start to paint by reseting all parameters and painting the
		///		first time.
		/// \details Internally the brush mask is cleared and centered.
		/// \param [in] _map The map on which is painted.
		/// \param [in] _obj An object from the database which is cloned into
		///		each new location.
		/// \param [in] _diameter Side length of the centralized quad-shaped brush.
		/// \param [in] _layer The layer from which objects might be deleted
		///		(REPLACE, DELETE) and in which new objects are added (ADD, REPLACE)
		///	\param [in] _mode \see Mode.
		void BeginPaint( Core::Map& _map, const Core::Object* _obj, int _diameter, int _x, int _y,
			int _layer, Mode _mode );

		/// \brief Paint into a circle centered at the given tile.
		void Paint( int _x, int _y );

		/// \brief Call after mouse button release to make sure nothing is
		///		drawn in the wrong moment
		void EndPaint()	{ m_map = nullptr; }

	private:
		Core::Map* m_map;	///< The induced map on which operations take place
		int m_diameter;		///< Diameter of the circle
		Mode m_mode;
		int m_layer;
		const Core::Object* m_obj;

		bool* m_mask;		///< Store for each tile if it was changed since last BeginPaint()
		int m_Xmin, m_Xmax, m_Ymin, m_Ymax;	///< Boundary coordinates for the mask

		/// \brief Tests if the given rect is inside the boundaries. If not
		///		the mask is resized.
		void CheckBoundaries( int _Xmin, int _Xmax, int _Ymin, int _Ymax );

		/// \brief Mark a certain point as edited
		void SetFlag( int _x, int _y );
		/// \brief Checks if the edited flag is set for a tile (_x, _y).
		bool WasEdited( int _x, int _y ) const;
	};

} // namespace Tools