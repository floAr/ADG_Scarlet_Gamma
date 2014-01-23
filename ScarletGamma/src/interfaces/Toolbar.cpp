#include "Toolbar.hpp"
#include "SFML/System/Vector2.hpp"
#include "Constants.hpp"

namespace Interfaces {

	Toolbar::Toolbar() :
		m_scrollPanel(*this),
		m_scrollLeft(*this),
		m_scrollRight(*this),
		m_widthSum(0.0f),
		m_scroll(0.0f)
	{
	}


	void Toolbar::Init(float _x, float _y, float _w, float _h)
	{
		// Cover the space of the whole toolbar
		Panel::setPosition( _x, _y );
		Panel::setSize( _w, _h );
		Panel::setBackgroundColor( sf::Color(50,50,50,150) );

		// Sub area for toolboxes
		m_scrollPanel->setPosition( 22.0f, 0.0f );
		m_scrollPanel->setSize( _w - 44.0f, _h );

		m_scrollLeft->load( "media/Black_ArrowLeft.png" );
		m_scrollLeft->setPosition( 0.0f, 0.0f );
		m_scrollLeft->setSize( 20.0f, _h );
		m_scrollLeft->setCallbackId( 1 );
		m_scrollLeft->bindCallbackEx( &Toolbar::BeginScroll, this, tgui::Picture::LeftMousePressed );
		m_scrollLeft->bindCallbackEx( &Toolbar::EndScroll, this, tgui::Picture::LeftMouseReleased );

		m_scrollRight->load( "media/Black_ArrowRight.png" );
		m_scrollRight->setPosition( _w - 20.0f, 0.0f );
		m_scrollRight->setSize( 20.0f, _h );
		m_scrollRight->setCallbackId( 2 );
		m_scrollRight->bindCallbackEx( &Toolbar::BeginScroll, this, tgui::Picture::LeftMousePressed );
		m_scrollRight->bindCallbackEx( &Toolbar::EndScroll, this, tgui::Picture::LeftMouseReleased );
	}


	void Toolbar::AddToolbox( tgui::Widget::Ptr _box )
	{
		// Add the real component and "recompute" global scrolling
		m_scrollPanel->add( _box );
		m_widthSum += _box->getSize().x + 2.0f;

		// Change the new components position that it is located at the right end.
		float x = 0.0f;
		auto& boxes = m_scrollPanel->getWidgets();
		if(boxes.size() > 1 )
			x = boxes[boxes.size()-2]->getPosition().x + boxes[boxes.size()-2]->getSize().x + 2.0f;
		_box->setPosition( x, 0.0f );
	}


	void Toolbar::Update( float _dt )
	{
		if( sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) )
		{
			// Move all boxes according to the scrolling
			if( m_scroll != 0.0f )
			{
				auto& boxes = m_scrollPanel->getWidgets();
				// A more or less constant speed, stop at the border.
				float value = m_scroll * _dt * 200.0f;
				if( boxes[0]->getPosition().x+value > 0.0f) {
					value = -boxes[0]->getPosition().x;
					m_scroll = 0.0f;
				}
				if( boxes[boxes.size()-1]->getPosition().x+boxes[boxes.size()-1]->getSize().x+value < m_scrollPanel->getSize().x ) {
					value = m_scrollPanel->getSize().x - (boxes[boxes.size()-1]->getPosition().x+boxes[boxes.size()-1]->getSize().x);
					m_scroll = 0.0f;
				}
				
				for( size_t i=0; i<boxes.size(); ++i )
				{
					boxes[i]->move( value, 0.0f );
				}
			}
		}
	}


	void Toolbar::BeginScroll(const tgui::Callback& _call)
	{
		if( m_scrollPanel->getWidgets().size() > 0 )
			m_scroll = _call.id == 1 ? -1.0f : 1.0f;
	}

	void Toolbar::EndScroll(const tgui::Callback& _call)
	{
		m_scroll = 0.0f;
	}






	MapToolbox::MapToolbox()
	{
		Panel::setSize( 150.0f, 100.0f );
		Panel::setBackgroundColor( sf::Color(50,250,50,150) );
	}

} // namespace Interfaces