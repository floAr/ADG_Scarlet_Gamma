#include "CircularMenu.hpp"
#include "Game.hpp"
#include "core\World.hpp"

namespace Interfaces {

	CircularMenu::CircularMenu( tgui::Gui& _gui ) :
		m_gui(_gui),
		m_circle(nullptr)
	{
		m_defaultButton->load( "media/Black.conf" );

		m_circle = tgui::Picture::Ptr(m_gui);
		m_circle->load( "media/circularmenu.png" );
		m_circle->setTransparency( 180 );
	}


	void CircularMenu::Clear()
	{
		// Remove the buttons with this menu
		for( size_t i = 0; i < m_buttons.size(); ++i )
			m_gui.remove( m_buttons[i] );

		m_buttons.clear();
	}


	CircularMenu::~CircularMenu()
	{
		Clear();
	}


	void CircularMenu::Show( const sf::Vector2f& _position, const std::vector<ObjInfo>& _objects )
	{
		// Remove old stuff
		Clear();

		// The more button the larger the circle
		float radius = 45.0f + _objects.size() * 3.0f;

		m_circle->setPosition( _position - sf::Vector2f(radius * 1.1f, radius * 1.1f) );
		m_circle->setSize( radius * 2.2f, radius * 2.2f );

		for( size_t i = 0; i < _objects.size(); ++i )
		{
			Core::Object* object = g_Game->GetWorld()->GetObject( _objects[i].id );
			tgui::Button::Ptr button = m_defaultButton.clone();
			m_gui.add(button);
			m_buttons.push_back(button);
			button->setSize( 50, 40 );
			button->setText( object->GetName() );
			button->setCallbackId( _objects[i].id );
			button->bindCallback( tgui::Button::LeftMouseClicked );
			PositionButton( _position, button, 360.0f / _objects.size() * i, radius );
			if( _objects[i].highlight ) {
				// Make fully opaque
				button->setTransparency(255);
			} else {
				button->setTransparency(150);
			}
		}
	}


	Core::ObjectID CircularMenu::GetClickedItem(float _x, float _y)
	{
		// Simulate a right click on gui (not natively supported from tgui)
		for( size_t i=0; i<m_buttons.size(); ++i )
		{
			if( m_buttons[i]->mouseOnWidget(_x, _y) )
			{
				return m_buttons[i]->getCallbackId();
			}
		}
		return 0xffffffff;
	}


	static const float DEGREE_TO_RADIANT = 0.01745329251f;
	void CircularMenu::PositionButton(const sf::Vector2f& _center, tgui::Button::Ptr _button, float _angle, float _radius)
	{
		// Put the button center on a position on the circle
		float bx = _center.x - _button->getSize().x*0.5f + _radius * sin(_angle * DEGREE_TO_RADIANT);
		float by = _center.y - _button->getSize().y*0.5f + _radius * cos(_angle * DEGREE_TO_RADIANT);
		_button->setPosition(bx, by);
	}
} // namespace Interfaces