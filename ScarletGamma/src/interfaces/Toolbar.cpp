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
		tgui::Panel::Ptr box = _box;
		box->setGlobalFont( getGlobalFont() );
		m_widthSum += _box->getSize().x + 2.0f;

		// Change the new components position that it is located at the right end.
		float x = 0.0f;
		auto& boxes = m_scrollPanel->getWidgets();
		if(boxes.size() > 1 )
			x = boxes[boxes.size()-2]->getPosition().x + boxes[boxes.size()-2]->getSize().x + 2.0f;
		_box->setPosition( x, 0.0f );
		Toolbox::Ptr(_box)->Init();
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
					value = std::min(0.0f, value);
					m_scroll = 0.0f;
				} else
				if( boxes[boxes.size()-1]->getPosition().x+boxes[boxes.size()-1]->getSize().x+value < m_scrollPanel->getSize().x ) {
					value = m_scrollPanel->getSize().x - (boxes[boxes.size()-1]->getPosition().x+boxes[boxes.size()-1]->getSize().x);
					value = std::max(0.0f, value);
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
			m_scroll = _call.id == 1 ? 1.0f : -1.0f;
	}

	void Toolbar::EndScroll(const tgui::Callback& _call)
	{
		m_scroll = 0.0f;
	}






	MapToolbox::MapToolbox()
	{
		Panel::setSize( 150.0f, 100.0f );
		Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	}

	void MapToolbox::Init()
	{
		tgui::Button::Ptr heading( *this );
		heading->load( "media/Black.conf" );
		heading->setPosition( 0.0f, 0.0f );
		heading->setText( STR_MAP );
		heading->setSize( 150.0f, 20.0f );
		heading->disable();
	}





	BrushToolbox::BrushToolbox()
	{
		Panel::setSize( 150.0f, 100.0f );
		Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	}

	void BrushToolbox::Init()
	{
		tgui::Button::Ptr heading( *this );
		heading->load( "media/Black.conf" );
		heading->setPosition( 0.0f, 0.0f );
		heading->setText( STR_BRUSH );
		heading->setSize( 150.0f, 20.0f );
		heading->disable();

		tgui::Label::Ptr lAction( *this );
		lAction->setTextColor( sf::Color(200, 200, 200) );
		lAction->setPosition( 0.0f, 23.0f );
		lAction->setText( STR_ACTION );
		lAction->setTextSize( 13 );
		lAction->setSize( 60.0f, 20.0f );
		tgui::ComboBox::Ptr action( *this );
		action->load( "media/Black.conf" );
		action->setPosition( 60.0f, 20.0f );
		action->setSize( 90.0f, 20.0f );
		action->addItem( STR_ADD );
		action->addItem( STR_REPLACE );
		action->addItem( STR_REMOVE );
		action->setSelectedItem( 0 );

		tgui::Label::Ptr lLayer( *this );
		lLayer->setTextColor( sf::Color(200, 200, 200) );
		lLayer->setPosition( 0.0f, 43.0f );
		lLayer->setText( STR_PROP_LAYER );
		lLayer->setTextSize( 13 );
		lLayer->setSize( 60.0f, 20.0f );
		tgui::ComboBox::Ptr layer( *this );
		layer->load( "media/Black.conf" );
		layer->setPosition( 60.0f, 40.0f );
		layer->setSize( 90.0f, 20.0f );

		tgui::Label::Ptr lName( *this );
		lName->setTextColor( sf::Color(200, 200, 200) );
		lName->setPosition( 0.0f, 63.0f );
		lName->setText( STR_PROP_NAME );
		lName->setTextSize( 13 );
		lName->setSize( 60.0f, 20.0f );
		tgui::EditBox::Ptr name( *this );
		name->load( "media/Black.conf" );
		name->setPosition( 60.0f, 60.0f );
		name->setSize( 90.0f, 20.0f );

		tgui::Label::Ptr lDiameter( *this );
		lDiameter->setTextColor( sf::Color(200, 200, 200) );
		lDiameter->setPosition( 0.0f, 83.0f );
		lDiameter->setText( STR_DIAMETER );
		lDiameter->setTextSize( 13 );
		lDiameter->setSize( 60.0f, 20.0f );
		tgui::Slider::Ptr diameter( *this );
		diameter->load( "media/Black.conf" );
		diameter->setVerticalScroll( false );
		diameter->setPosition( 69.0f, 84.0f );
		diameter->setSize( 72.0f, 12.0f );
	}





	PlayersToolbox::PlayersToolbox()
	{
		Panel::setSize( 150.0f, 100.0f );
		Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	}

	void PlayersToolbox::Init()
	{
		tgui::Button::Ptr heading( *this );
		heading->load( "media/Black.conf" );
		heading->setPosition( 0.0f, 0.0f );
		heading->setText( STR_PLAYER );
		heading->setSize( 150.0f, 20.0f );
		heading->disable();
	}





	NPCToolbox::NPCToolbox()
	{
		Panel::setSize( 150.0f, 100.0f );
		Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	}

	void NPCToolbox::Init()
	{
		tgui::Button::Ptr heading( *this );
		heading->load( "media/Black.conf" );
		heading->setPosition( 0.0f, 0.0f );
		heading->setText( STR_NPC );
		heading->setSize( 150.0f, 20.0f );
		heading->disable();
	}

} // namespace Interfaces