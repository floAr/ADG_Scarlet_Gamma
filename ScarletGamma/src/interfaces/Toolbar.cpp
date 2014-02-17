#include "Toolbar.hpp"
#include "SFML/System/Vector2.hpp"
#include "Constants.hpp"
#include "Game.hpp"
#include "core/World.hpp"
#include "states/CommonState.hpp"

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
		Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	//	Panel::bindCallback( &Toolbar::unfocus, this, tgui::Panel::ke

		// Sub area for toolboxes
		m_scrollPanel->setPosition( 22.0f, 0.0f );

		m_scrollLeft->load( "media/Black_ArrowLeft.png" );
		m_scrollLeft->setPosition( 0.0f, 0.0f );
		m_scrollLeft->setCallbackId( 1 );
		m_scrollLeft->bindCallbackEx( &Toolbar::BeginScroll, this, tgui::Picture::LeftMousePressed );
		m_scrollLeft->bindCallbackEx( &Toolbar::EndScroll, this, tgui::Picture::LeftMouseReleased );

		m_scrollRight->load( "media/Black_ArrowRight.png" );
		m_scrollRight->setCallbackId( 2 );
		m_scrollRight->bindCallbackEx( &Toolbar::BeginScroll, this, tgui::Picture::LeftMousePressed );
		m_scrollRight->bindCallbackEx( &Toolbar::EndScroll, this, tgui::Picture::LeftMouseReleased );

		setSize( _w, _h );
	}


	int Toolbar::AddToolbox( tgui::Widget::Ptr _box )
	{
		// Add the real component and "recompute" global scrolling
		m_scrollPanel->add( _box );
		tgui::Panel::Ptr box = _box;
		box->setGlobalFont( getGlobalFont() );

		// Change the new components position that it is located at the right end.
		float x = 0.0f;
//		auto& boxes = m_scrollPanel->getWidgets();
//		if(boxes.size() > 1 )
//			x = boxes[boxes.size()-2]->getPosition().x + boxes[boxes.size()-2]->getSize().x + 2.0f;
		_box->setPosition( m_widthSum + 2.0f, 0.0f );
		m_widthSum += _box->getSize().x + 2.0f;
		Toolbox::Ptr(_box)->Init();

		return m_scrollPanel->getWidgets().size()-1;
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
				if( (value > 0) && boxes[0]->getPosition().x+value >= 2.0f ) {
					// If leftmost box would be moved too far -> stop.
					value = -boxes[0]->getPosition().x + 2.0f;
					value = std::max(0.0f, value);
					m_scroll = 0.0f;
				} else
				if( (value < 0) && boxes[boxes.size()-1]->getPosition().x+boxes[boxes.size()-1]->getSize().x+value < m_scrollPanel->getSize().x ) {
					// Rightmost box would be moved too far -> stop.
					value = m_scrollPanel->getSize().x - (boxes[boxes.size()-1]->getPosition().x+boxes[boxes.size()-1]->getSize().x);
					value = std::min(0.0f, value);
					m_scroll = 0.0f;
				}
				
				for( size_t i=0; i<boxes.size(); ++i )
				{
					boxes[i]->move( value, 0.0f );
				}
			}
		}

		// Update the toolboxes
		for( size_t i=0; i<m_scrollPanel->getWidgets().size(); ++i )
		{
			((Toolbox::Ptr)m_scrollPanel->getWidgets()[i])->Update( _dt );
		}
	}


	void Toolbar::SetBoxVisiblity( int _index, bool _visible )
	{
		auto& boxes = m_scrollPanel->getWidgets();
		float offset = 0.0f;
		// Hide or show the box. Its width is the amount of space the other
		// elements must be moved
		if( _visible && !boxes[_index]->isVisible())
		{
			boxes[_index]->show();
			offset = boxes[_index]->getSize().x + 2.0f;
			m_widthSum += offset;
		} else if( !_visible && boxes[_index]->isVisible())
		{
			boxes[_index]->hide();
			offset = -boxes[_index]->getSize().x - 2.0f;
			m_widthSum += offset;
		}
		// Move succeeding elements
		for( size_t i=_index+1; i<boxes.size(); ++i )
		{
			boxes[i]->move(offset, 0.0f);
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


	void Toolbar::unfocus()
	{
		Panel::unfocus();
		m_Parent->unfocus();
	}

	void Toolbar::setSize(float _width, float _height)
	{
		Panel::setSize( _width, _height );
		m_scrollPanel->setSize( _width - 44.0f, _height );
		m_scrollLeft->setSize( 20.0f, _height );
		m_scrollRight->setPosition( _width - 20.0f, 0.0f );
		m_scrollRight->setSize( 20.0f, _height );
	}





	
	void Toolbox::unfocus()
	{
		Panel::unfocus();
		m_Parent->unfocus();
	}






	MapToolbox::MapToolbox() :
		m_selected(0),
		m_mapList(nullptr),
		m_newNameEdit(nullptr)
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
		
		m_mapList = tgui::ListBox::Ptr( *this );
		m_mapList->load( "media/Black.conf" );
		m_mapList->setSize( 150.0f, 60.0f );
		m_mapList->setPosition( 0.0f, 20.0f );
		m_mapList->setItemHeight( 19 );
		m_mapList->getScrollbar()->setSize( 12.0f, m_mapList->getScrollbar()->getSize().y );
		m_mapList->bindCallbackEx( [this](const tgui::Callback& _caller){
			if(_caller.value != -1) m_selected = _caller.value;
			else m_mapList->setSelectedItem( m_selected );
		}, tgui::ListBox::ItemSelected );
		// Bad global reference (g_Game) but best solution if no parameters can be given.
		m_maps = g_Game->GetWorld()->GetAllMaps();
		for( size_t i=0; i<m_maps.size(); ++i )
			m_mapList->addItem( g_Game->GetWorld()->GetMap(m_maps[i])->GetName() );
		if( m_maps.size() > 0 )
			m_mapList->setSelectedItem( 0 );

		m_newNameEdit = tgui::EditBox::Ptr( *this );
		m_newNameEdit->load( "media/Black.conf" );
		m_newNameEdit->setPosition( 0.0f, 80.0f );
		m_newNameEdit->setSize( 110.0f, 20.0f );
		tgui::Button::Ptr addbtn( *this );
		addbtn->load( "media/Black.conf" );
		addbtn->setPosition( 110.0f, 80.0f );
		addbtn->setSize( 20.0f, 20.0f );
		addbtn->bindCallback(&MapToolbox::AddMap, this, tgui::Button::LeftMouseClicked);
		addbtn->setText( "+" );
	}

	void MapToolbox::AddMap()
	{
		if( !m_newNameEdit->getText().isEmpty() )
		{
			Core::MapID id = g_Game->GetWorld()->NewMap( m_newNameEdit->getText(), 1, 1 );
			m_mapList->addItem( m_newNameEdit->getText() );
			m_selected = m_maps.size();
			m_mapList->setSelectedItem( m_selected );
			m_maps.push_back( id );
			m_newNameEdit->setText( STR_EMPTY );
		}
	}

	void MapToolbox::SetMap(Core::MapID _id)
	{
		// Find the map in the vector and store the index
		int selected = std::find(m_maps.begin(), m_maps.end(), _id) - m_maps.begin();
		if( selected < (int)m_maps.size() )
			m_selected = selected;
	}






	ModeToolbox::ModeToolbox() :
		m_selected(0)
	{
		Panel::setSize( 95.0f, 100.0f );
		Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	}

	void ModeToolbox::Init()
	{
		tgui::Button::Ptr heading( *this );
		heading->load( "media/Black.conf" );
		heading->setPosition( 0.0f, 0.0f );
		heading->setText( STR_MODE );
		heading->setSize( getSize().x, 20.0f );
		heading->disable();

		m_actionList = tgui::ListBox::Ptr( *this );
		m_actionList->load( "media/Black.conf" );
		m_actionList->setSize( getSize().x, 80.0f );
		m_actionList->setPosition( 0.0f, 20.0f );
		m_actionList->setItemHeight( 19 );
		m_actionList->removeScrollbar();// getScrollbar()->setSize( 12.0f, m_mapList->getScrollbar()->getSize().y );
		m_actionList->bindCallbackEx( &ModeToolbox::SelectMode, this, tgui::ListBox::ItemSelected );
		m_actionList->addItem( STR_SELECTION );
		m_actionList->addItem( STR_BRUSH );
		m_actionList->addItem( STR_ACTION );
		m_actionList->addItem( STR_DRAGNDROP );
		m_actionList->setSelectedItem( 0 );

		// Add dependent boxes
		Toolbar* bar = dynamic_cast<Toolbar*>(m_Parent->getParent());
		m_brushIndex = bar->AddToolbox( m_brushBox );
		bar->SetBoxVisiblity( m_brushIndex, false );	// This one is not visible initially
	}

	void ModeToolbox::SelectMode(const tgui::Callback& _call)
	{
		// No deselection possible
		if( _call.value == -1 )
		{
			m_actionList->setSelectedItem( m_selected );
			return;
		}

		Toolbar* bar = dynamic_cast<Toolbar*>(m_Parent->getParent());
		// First hide all toolboxes (so nothing gets forgotten)
		bar->SetBoxVisiblity( m_brushIndex, false );
		// Hide/show depending toolboxes
		switch( _call.value )
		{
		case 1:
			bar->SetBoxVisiblity( m_brushIndex, true );
			break;
		}

		// Standard action: store which one is selected.
		m_selected = _call.value;
	}






	BrushToolbox::BrushToolbox() :
		m_actionChoice(nullptr),
		m_layer(nullptr),
		m_diameter(nullptr)
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
		m_actionChoice = tgui::ComboBox::Ptr( *this );
		m_actionChoice->load( "media/Black.conf" );
		m_actionChoice->setPosition( 60.0f, 20.0f );
		m_actionChoice->setSize( 90.0f, 20.0f );
		m_actionChoice->addItem( STR_ADD );
		m_actionChoice->addItem( STR_REPLACE );
		m_actionChoice->addItem( STR_REMOVE );
		m_actionChoice->setSelectedItem( 1 );

		tgui::Label::Ptr lLayer( *this );
		lLayer->setTextColor( sf::Color(200, 200, 200) );
		lLayer->setPosition( 0.0f, 43.0f );
		lLayer->setText( STR_PROP_LAYER );
		lLayer->setTextSize( 13 );
		lLayer->setSize( 60.0f, 20.0f );
		m_layer = tgui::ComboBox::Ptr( *this );
		m_layer->load( "media/Black.conf" );
		m_layer->setPosition( 60.0f, 40.0f );
		m_layer->setSize( 90.0f, 20.0f );
		m_layer->addItem( STR_LAYER0 );
		m_layer->addItem( STR_LAYER1 );
		m_layer->addItem( STR_LAYER2 );
		m_layer->addItem( STR_LAYER3 );
		m_layer->addItem( STR_LAYER4 );
		m_layer->addItem( STR_LAYER5 );
		m_layer->addItem( STR_LAYER6 );
		m_layer->addItem( STR_LAYER7 );
		m_layer->addItem( STR_LAYER8 );
		m_layer->addItem( STR_LAYER9 );
	//	m_layer->addItem( STR_ONTOP );
		m_layer->setSelectedItem( 0 );

		tgui::Label::Ptr lDiameter( *this );
		lDiameter->setTextColor( sf::Color(200, 200, 200) );
		lDiameter->setPosition( 0.0f, 63.0f );
		lDiameter->setText( STR_DIAMETER );
		lDiameter->setTextSize( 13 );
		lDiameter->setSize( 60.0f, 20.0f );
		m_diameter = tgui::Slider::Ptr( *this );
		m_diameter->load( "media/Black.conf" );
		m_diameter->setVerticalScroll( false );
		m_diameter->setPosition( 69.0f, 64.0f );
		m_diameter->setSize( 72.0f, 12.0f );
		m_diameter->setMinimum( 1 );
	}

	int BrushToolbox::GetDiameter() const
	{
		return m_diameter->getValue();
	}
	
	int BrushToolbox::GetLayer() const
	{
		return m_layer->getSelectedItemIndex();
	}
	
	Tools::Brush::Mode BrushToolbox::GetMode() const
	{
		return Tools::Brush::Mode(m_actionChoice->getSelectedItemIndex());
	}





	PlayersToolbox::PlayersToolbox() :
		m_lastUpdate(0.0f)
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

		m_playerList = tgui::ListBox::Ptr( *this );
		m_playerList->load( "media/Black.conf" );
		m_playerList->setSize( 150.0f, 80.0f );
		m_playerList->setPosition( 0.0f, 20.0f );
		m_playerList->setItemHeight( 19 );
		m_playerList->getScrollbar()->setSize( 12.0f, m_playerList->getScrollbar()->getSize().y );
		
		// WTF would like to bind list - not possible
		bindCallbackEx( &PlayersToolbox::DragPlayer, this, tgui::Panel::LeftMousePressed );
		bindCallbackEx( &PlayersToolbox::JumpToPlayer, this, tgui::Panel::LeftMouseClicked );
	}

	void PlayersToolbox::Update( float _dt )
	{
		m_lastUpdate += _dt;
		if( m_lastUpdate > 2.0f )
		{
			m_lastUpdate = 0.0f;
			m_playerList->removeAllItems();
			// Test all possible player ids
			auto& players = g_Game->GetWorld()->GetAllPlayers();
			for( size_t i=0; i<players.size(); ++i )
			{
				Core::Object* player = g_Game->GetWorld()->GetObject( players[i] );
				if( player )
					m_playerList->addItem( player->GetProperty( STR_PROP_NAME ).Value() );
			}
		}
	}

	Core::Object* PlayersToolbox::GetPlayer( float _x, float _y )
	{
		// Force the list to select something
		m_playerList->leftMousePressed( _x, _y );

		return g_Game->GetWorld()->FindPlayer( m_playerList->getSelectedItem() );
	}

	void PlayersToolbox::DragPlayer(const tgui::Callback& _caller)
	{
		// Get which player must be dragged from list
		Core::Object* player = GetPlayer( (float)_caller.mouse.x, (float)_caller.mouse.y );

		if( player )
		{
			if( !*m_dragNDropHandler )
				*m_dragNDropHandler = new DragContent();
			(*m_dragNDropHandler)->object = player;
			(*m_dragNDropHandler)->from = DragContent::PLAYERS_LIST;
			(*m_dragNDropHandler)->prop = nullptr;
		} else if( *m_dragNDropHandler )
		{
			delete *m_dragNDropHandler;
			*m_dragNDropHandler = nullptr;
		}
	}

	void PlayersToolbox::JumpToPlayer(const tgui::Callback& _caller)
	{
		// Get which player was clicked
		Core::Object* player = GetPlayer( (float)_caller.mouse.x, (float)_caller.mouse.y );

		// Call the CommonState's GoTo
		m_goto(player);
	}






	GoToToolbox::GoToToolbox() :
		m_eX(nullptr),
		m_eY(nullptr),
		m_eID(nullptr)
	{
		Panel::setSize( 150.0f, 100.0f );
		Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	}

	void GoToToolbox::Init()
	{
		tgui::Button::Ptr heading( *this );
		heading->load( "media/Black.conf" );
		heading->setPosition( 0.0f, 0.0f );
		heading->setText( STR_GOTO );
		heading->setSize( 150.0f, 20.0f );
		heading->disable();

		// Enter x and y of a position
		tgui::Label::Ptr lPosition( *this );
		lPosition->setText("Position:");
		lPosition->setPosition(3.0f, 23.0f);
		lPosition->setTextSize(16);
		lPosition->setTextColor( sf::Color(200,200,200) );
		m_eX = tgui::EditBox::Ptr( *this );
		m_eX->load( "media/Black.conf" );
		m_eX->setPosition( 0.0f, 40.0f );
		m_eX->setSize( 55.0f, 20.0f );
		m_eX->setNumbersOnly( true );
		m_eY = m_eX.clone();	this->add(m_eY);
		m_eY->setPosition(55.0f, 40.0f);
		tgui::Button::Ptr gobtn1( *this );
		gobtn1->load( "media/Black.conf" );
		gobtn1->setPosition( 110.0f, 40.0f );
		gobtn1->setSize( 20.0f, 20.0f );
		gobtn1->setText( "->" );

		// Enter an object id
		tgui::Label::Ptr lId( *this );
		lId->setText("Objekt (ID):");
		lId->setPosition(3.0f, 63.0f);
		lId->setTextSize(16);
		lId->setTextColor( sf::Color(200,200,200) );
		m_eID = m_eX.clone();	this->add(m_eID);
		m_eID->setPosition(0.0f, 80.0f);
		m_eID->setSize( 110.0f, 20.0f );
		tgui::Button::Ptr gobtn2 = gobtn1.clone(); this->add( gobtn2 );
		gobtn2->setPosition( 110.0f, 80.0f );

		// Bind all callbacks after cloning (otherwise elements could have multiple methods)
		gobtn1->bindCallback( &GoToToolbox::GoToPosition, this, tgui::Button::LeftMouseClicked );
		gobtn2->bindCallback( &GoToToolbox::GoToObject, this, tgui::Button::LeftMouseClicked );
		m_eX->bindCallback( &GoToToolbox::GoToPosition, this, tgui::EditBox::ReturnKeyPressed );
		m_eY->bindCallback( &GoToToolbox::GoToPosition, this, tgui::EditBox::ReturnKeyPressed );
		m_eID->bindCallback( &GoToToolbox::GoToObject, this, tgui::EditBox::ReturnKeyPressed );
	}

	void GoToToolbox::GoToPosition()
	{
		int x = atoi(m_eX->getText().toAnsiString().c_str());
		int y = atoi(m_eY->getText().toAnsiString().c_str());
		g_Game->GetCommonState()->GetStateView().setCenter(sf::Vector2f(x * float(TILESIZE), y * float(TILESIZE)));
	}

	void GoToToolbox::GoToObject()
	{
		Core::ObjectID id = atoi(m_eID->getText().toAnsiString().c_str());
		Core::Object* object =  g_Game->GetWorld()->GetObject( id );
		if( object )
			m_goto( object );
		else
			g_Game->AppendToChatLog( Network::ChatMsg(STR_WRONG_ID, sf::Color::Red) );
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


	CombatToolbox::CombatToolbox()
		: m_combatButton(nullptr)
	{
		Panel::setSize( 150.0f, 100.0f );
		Panel::setBackgroundColor( sf::Color(50,50,50,150) );
	}

	void CombatToolbox::Init()
	{
		tgui::Button::Ptr heading( *this );
		heading->load( "media/Black.conf" );
		heading->setPosition( 0.0f, 0.0f );
		heading->setText( STR_COMBAT );
		heading->setSize( 150.0f, 20.0f );
		heading->disable();

		// Button
		m_combatButton = tgui::Button::Ptr( *this );
		m_combatButton->load( "media/Black.conf" );
		m_combatButton->setPosition( 0.0f, 23.0f );
		m_combatButton->setSize( 150.0f, 20.0f );
		m_combatButton->setTextSize(16);
		m_combatButton->setText( "Kampf beginnen" );
		m_combatButton->bindCallback( &CombatToolbox::ButtonClicked, this, tgui::Button::LeftMouseClicked );
	}

	void CombatToolbox::ButtonClicked()
	{
		// TODO: implement
	}

} // namespace Interfaces