#include "Brush.hpp"
#include <assert.h>
#include "Game.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.hpp"

namespace Tools {

	Brush::Brush() :
		m_map(nullptr),
		m_mask(nullptr),
		m_Xmin(0),
		m_Xmax(19),
		m_Ymin(0),
		m_Ymax(19),
		m_diameter(0)
	{
		// Allocate space for a 20x20 painting
		m_mask = new bool[400];
	}


	Brush::~Brush()
	{
		delete[] m_mask;
	}


	void Brush::BeginPaint( Core::Map& _map, const Core::Object* _obj, int _diameter, int _x, int _y,
		int _layer, Mode _mode )
	{
		// Copy all parameters for the painting process
		m_map = &_map;
		m_diameter = _diameter;
		assert( _diameter > 0 );
		m_mode = _mode;
		m_layer = _layer;
		m_obj = _obj;

		// Clear and center mask
		int width = (m_Xmax-m_Xmin+1);
		int height = (m_Ymax-m_Ymin+1);
		memset( m_mask, 0, sizeof(bool) * width * height );
		m_Xmin = _x - width/2;
		m_Xmax = m_Xmin + width - 1;
		m_Ymin = _y - height/2;
		m_Ymax = m_Ymin + height - 1;

		Paint( _x, _y );
	}


	void Brush::Paint( int _x, int _y )
	{
		// Begin was not called
		if( !m_map ) return;

		// Split the diameter (for odd numbers make radius in positive
		// direction larger.
		int r0 = m_diameter / 2, r1 = m_diameter - r0;
		// Resize the area where we are going to edit if required.
		CheckBoundaries(_x-r0, _x+r1, _y-r0, _y+r1);

		float rsq = m_diameter * m_diameter / 4.0f;				// Real radius squared to compute circle distances
	//	float cx = ((m_diameter & 1) == 1) ? _x : (_x+0.5f);	// Translated center for even coordinates
	//	float cy = ((m_diameter & 1) == 1) ? _y : (_y+0.5f);	// Translated center for even coordinates
		for( int y = _y-r0; y <_y+r1; ++y )
		{
			for( int x = _x-r0; x <_x+r1; ++x )
			{
				// Already flagged?
				if( !WasEdited(x, y) )
				{
					// Inside circle? - Current decision: rectangle is preferred as long as the shape is not a player choice
					//if( (x-cx)*(x-cx) + (y-cy)*(y-cy) <= rsq )
					//{

					SetFlag( x, y );
					// Now edit the map
					switch( m_mode )
					{
					case Brush::ADD:
						m_map->Add( g_Game->GetWorld()->NewObject( m_obj ), x, y, m_layer );
						break;

					case Brush::REPLACE:
						// Search objects which are in the target layer and remove them.
						for( int i=0; i<m_map->GetObjectsAt(x,y).Size(); ++i )
						{
							Core::ObjectID id =  m_map->GetObjectsAt(x,y)[i];
							if( m_layer == g_Game->GetWorld()->GetObject( id )->GetProperty( STR_PROP_LAYER ).Evaluate() )
							{
								m_map->Remove( m_map->GetObjectsAt(x,y)[i] );
								g_Game->GetWorld()->RemoveObject( id );	// Assumes real deletion
								// The currently iterated array should be shorter now.
								--i;
							}
						}
						m_map->Add( g_Game->GetWorld()->NewObject( m_obj ), x, y, m_layer );
						break;
					case Brush::DELETE:
						// Search objects which are in the target layer AND
						// have the same name as the reference object.
						for( int i=0; i<m_map->GetObjectsAt(x,y).Size(); ++i )
						{
							Core::ObjectID id =  m_map->GetObjectsAt(x,y)[i];
							Core::Object* obj = g_Game->GetWorld()->GetObject( id );
							if( m_layer == obj->GetProperty( STR_PROP_LAYER ).Evaluate() &&
								obj->GetName() == m_obj->GetName() )
							{
								m_map->Remove( m_map->GetObjectsAt(x,y)[i] );
								g_Game->GetWorld()->RemoveObject( id );	// Assumes real deletion
								// The currently iterated array should be shorter now.
								--i;
							}
						}
					} // switch
				} // if !WasEdited
			} // for x
		} // for y
	}


	void Brush::CheckBoundaries( int _Xmin, int _Xmax, int _Ymin, int _Ymax )
	{
		// Compute if the required range goes out of current area.
		assert( _Xmin < _Xmax );
		assert( _Ymin < _Ymax );
		int _nx = std::max(0, m_Xmin - _Xmin);
		int _px = std::max(0, _Xmax - m_Xmax);
		int _ny = std::max(0, m_Ymin - _Ymin);
		int _py = std::max(0, _Ymax - m_Ymax);

		if( _nx+_px+_ny+_py == 0 ) return;	// Nothing would happen

		// Allocate a new larger memory
		int width = (m_Xmax-m_Xmin+1);
		int height = (m_Ymax-m_Ymin+1);
		int newWidth = width + _nx + _px;
		int newHeight = height + _ny + _py;
		bool* mask = new bool[newWidth * newHeight];
		memset(mask, 0, sizeof(bool) * newWidth * newHeight);
		// Copy old part
		for( int y=0; y<height; ++y ) {
			for( int x=0; x<width; ++x ) {
				mask[x+_nx+(y+_ny)*newWidth] = m_mask[x+y*width];
			}
		}
		// The delete should delete not more than the memory block of the array
		// itself. The rest is moved to the new one.
		delete[] m_mask;

		m_mask = mask;
		m_Xmin -= _nx;
		m_Ymin -= _ny;
		m_Xmax += _px;
		m_Ymax += _py;
	}


	void Brush::SetFlag( int _x, int _y )
	{
		m_mask[(_x-m_Xmin) + (_y-m_Ymin) * (m_Xmax - m_Xmin + 1)] = true;
	}


	bool Brush::WasEdited( int _x, int _y ) const
	{
		return m_mask[(_x-m_Xmin) + (_y-m_Ymin) * (m_Xmax - m_Xmin + 1)];
	}

} // namespace Tools