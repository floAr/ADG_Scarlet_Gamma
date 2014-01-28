#include "states/MasterState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "states/StateMachine.hpp"
#include "Constants.hpp"
#include "utils/Falloff.hpp"
#include "network/Messenger.hpp"
#include "interfaces/DragNDrop.hpp"
#include "actions/ActionPool.hpp"
#include <iostream>
#include "core/PredefinedProperties.hpp"
#include "network/WorldMessages.hpp"

using namespace Core;

namespace States {

	MasterState::MasterState( const std::string& _loadFile ) :
		m_propertyPanel(nullptr),
		m_dbProperties(nullptr),
		m_dbModules(nullptr),
		m_dbTemplates(nullptr),
		m_modulePanel(nullptr),
		m_objectsPanel(nullptr),
		m_selectionView(nullptr),
		m_toolbar(nullptr),
		m_hiddenLayers(10, 0),
		m_worldFileName(_loadFile)
	{
		// Load the map
		Jo::Files::HDDFile file(_loadFile);
		g_Game->GetWorld()->Load( file );

		// Load the template databases
		m_dbProperties = new Core::World();
		if( !Jo::Files::Utils::Exists( "data/properties.dat" ) )
			CreateDefaultPropertyBase();
		else {
			Jo::Files::HDDFile database( "data/properties.dat" );
			m_dbProperties->Load( database );
		}
		m_dbModules = new Core::World();
		if( !Jo::Files::Utils::Exists( "data/modules.dat" ) )
			CreateDefaultModuleBase();
		else {
			Jo::Files::HDDFile database( "data/modules.dat" );
			m_dbModules->Load( database );
		}
		m_dbTemplates = new Core::World();
		if( !Jo::Files::Utils::Exists( "data/objects.dat" ) )
			CreateDefaultTemplateBase();
		else {
			Jo::Files::HDDFile database( "data/objects.dat" );
			m_dbModules->Load( database );
		}

		m_viewPanel = Interfaces::PropertyPanel::Ptr(m_gui);
		m_viewPanel->Init( 240.0f, 384.0f, 240.0f, 384.0f, false, false, 0, &m_draggedContent );

		// Load properties from database into gui
		m_propertyPanel = Interfaces::PropertyPanel::Ptr(m_gui);
		m_propertyPanel->Init( 0.0f, 0.0f, 240.0f, 384.0f, true, false, 0, &m_draggedContent );
		// The all-properties object has ID 0
		m_propertyPanel->Show( m_dbProperties->GetObject( 0 ) );

		// Load Modules from database into gui
		m_modulePanel = Interfaces::ObjectPanel::Ptr(m_gui);
		m_modulePanel->Init( 0.0f, 384.0f, 240.0f, 384.0f, true, m_dbModules, Interfaces::DragContent::MODULES_PANEL, &m_draggedContent, m_viewPanel );

		// Load Objects from database into gui
		m_objectsPanel = Interfaces::ObjectPanel::Ptr(m_gui);
		m_objectsPanel->Init( 240.0f, 0.0f, 240.0f, 384.0f, true, m_dbTemplates, Interfaces::DragContent::OBJECT_PANEL, &m_draggedContent, m_viewPanel );

		// The temporary selection menu
		m_selectionView = Interfaces::PropertyPanel::Ptr(m_gui);
		m_selectionView->Init(624.0f, 100.0f, 400.0f, 428.0f, false, false, 0, &m_draggedContent);
		m_selectionView->hide();

		m_toolbar = Interfaces::Toolbar::Ptr(m_gui);
		m_toolbar->Init( 480.0f, 0.0f, 544.0f, 100.0f );
		m_toolbar->AddToolbox( m_mapTool );
		m_toolbar->AddToolbox( m_modeTool );
		m_toolbar->AddToolbox( Interfaces::PlayersToolbox::Ptr() );
		m_toolbar->AddToolbox( Interfaces::NPCToolbox::Ptr() );

		// Set chat color...
		m_color = sf::Color(80,80,250);
		m_name = "Master";

		//set up m_hiddenLayers for 10 layers
		/*int l;
		for(l=0;l<10;l++)
		{
			m_hiddenLayers.push_back(0);
		}*/
	}

	void MasterState::Update(float dt)
	{
		CommonState::Update(dt);

		g_Game->GetWorld()->Update(dt);

		m_toolbar->Update( dt );

		// Repair selection
		for( int i=0; i<m_selection.Size(); ++i )
		{
			Core::Object* obj = g_Game->GetWorld()->GetObject( m_selection[i] );
			// If removed from map or from game delete!
			if( !obj || !obj->IsLocatedOnAMap() )
			{
				m_selection.Remove( m_selection[i--] );
				m_selectionChanged = true;
			}
		}

		// For empty selections hide the selection view
		if( m_selectionChanged )
		{
			if( m_selection.Size() > 0 )
			{
				m_selectionView->show();
			} else m_selectionView->hide();
			m_selectionChanged = false;
		}

		// Update the viewed properties
		if( m_selectionView->isVisible() )
			m_selectionView->Show( g_Game->GetWorld(), m_selection );

	}

	void MasterState::Draw(sf::RenderWindow& win)
	{
		// Draw some color to the background
		static sf::Color c(20, 26, 36);
		win.clear(c);

		// Render
		if( GetCurrentMap() )
		{
			Graphics::TileRenderer::Render(win, *GetCurrentMap(),
				[](Core::Map&,sf::Vector2i&){ return 1.0f; },
				(const bool*)(&m_hiddenLayers[0]));
		}

		// If the selected object has a path draw it
		for( int i=0; i<m_selection.Size(); ++i )
			DrawPathOverlay(win, g_Game->GetWorld()->GetObject(m_selection[i]));

		Graphics::TileRenderer::RenderSelection( win, m_selection );

		GameState::Draw(win);
	}


	void MasterState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled)
	{
		// Return if the GUI already handled it
		if (guiHandled)
			return;

		// The element was dropped unregistered (e.g. outside window)
		if( m_draggedContent )
		{
			delete m_draggedContent;
			m_draggedContent = nullptr;
		}

		int tileX = (int)floor(tilePos.x);
		int tileY = (int)floor(tilePos.y);
		switch (button.button)
		{
		case sf::Mouse::Left: {
			if( m_modeTool->GetMode() == Interfaces::ModeToolbox::BRUSH )
			{
				if( !m_objectsPanel->GetSelected() ) return;// TODO: Warnung in chat
				// Paint new objects with the brush.
				m_brush.BeginPaint( *GetCurrentMap(),
					m_objectsPanel->GetSelected(),
					m_modeTool->Brush()->GetDiameter(),
					tileX, tileY,
					m_modeTool->Brush()->GetLayer(),
					m_modeTool->Brush()->GetMode() );
			}
			if( m_modeTool->GetMode() == Interfaces::ModeToolbox::SELECTION )
			{
				//TODO Start rect-selection
				m_rectSelectionStart=tilePos;
				m_rectSelection=true;
			}
							  } break;
		case sf::Mouse::Right: {
			if( GetCurrentMap()->GetObjectsAt(tileX, tileY).Size() > 0 )
			{
				SelectionState* gs = dynamic_cast<SelectionState*>(g_Game->GetStateMachine()->PushGameState(GST_SELECTION));
				gs->SetTilePosition(tileX, tileY);
			}

							   } break;
		}
	}

	void MasterState::MouseButtonReleased( sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, float time, bool guiHandled)
	{
		// Always a good idea to stop painting
		m_brush.EndPaint();

		// Return if the GUI already handled it
		if (guiHandled)
			return;

		// Handle drop-event of drag&drop action
		if( m_draggedContent )
		{
			// Where is the element dropped?
			if( m_objectsPanel->mouseOnWidget( (float)button.x, (float)button.y ) )
			{
				m_objectsPanel->HandleDropEvent();
			} else if( m_modulePanel->mouseOnWidget( (float)button.x, (float)button.y ) )
			{
				m_modulePanel->HandleDropEvent();
			} else if( m_viewPanel->mouseOnWidget( (float)button.x, (float)button.y ) )
			{
				m_viewPanel->HandleDropEvent();
			} else if( m_propertyPanel->mouseOnWidget( (float)button.x, (float)button.y ) )
			{
				// Just ignore things dragged to the property list
			} else {	// Things were dragged to the map.
				if( m_draggedContent->from == Interfaces::DragContent::OBJECT_PANEL )
				{
					if( !GetCurrentMap() )
					{
						Network::ChatMsg(STR_MSG_CREATE_MAP_FIRST, sf::Color::Red).Send();
					} else {
						// Insert object copy to the map
						ObjectID id = g_Game->GetWorld()->NewObject( m_draggedContent->object );

						int x = (int)floor(tilePos.x);
						int y = (int)floor(tilePos.y);
						// Meaningful layer: on top
						int l = GetCurrentMap()->GetObjectsAt(x,y).Size();
						GetCurrentMap()->Add( id, x, y, l );
					}
				}
			}

			delete m_draggedContent;
			m_draggedContent = nullptr;
		}

		if(m_rectSelection){
			int sX,sY,minX,minY,maxX,maxY;
			if(tilePos.x<m_rectSelectionStart.x)
			{
				minX = (int)tilePos.x;
				maxX = (int)m_rectSelectionStart.x;
			}
			else
			{
				maxX = (int)tilePos.x;
				minX = (int)m_rectSelectionStart.x;
			}
			if(tilePos.y<m_rectSelectionStart.y)
			{
				minY = (int)tilePos.y;
				maxY = (int)m_rectSelectionStart.y;
			}
			else
			{
				maxY = (int)tilePos.y;
				minY = (int)m_rectSelectionStart.y;
			}
			SelectionState* gs = dynamic_cast<SelectionState*>(g_Game->GetStateMachine()->PushGameState(GST_SELECTION));
			for(sX = minX; sX < maxX; sX++)
			{
				for(sY = minY; sY < maxY; sY++)
				{
					//TODO: Do selection stuff here
				}

			}


			m_rectSelection=false;
		}
	}


	void MasterState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
	{
		// Return if the GUI already handled it
		if (guiHandled)
			return;

		// Let common state handle input
		CommonState::KeyPressed(key, guiHandled);

		switch(key.code)
		{
			//on alt clear the current mask
		case sf::Keyboard::LAlt:
			int l;
			for(l = 0; l < 10; l++)
			{
				m_hiddenLayers[l] = 0;
			}
			m_firstLayerSelection=true;
			break;
			//for each key add the mask, as long as alt is pressed (maybe cache this in local field)
		case sf::Keyboard::Num1:
		case sf::Keyboard::Num2:
		case sf::Keyboard::Num3:
		case sf::Keyboard::Num4:
		case sf::Keyboard::Num5:
		case sf::Keyboard::Num6:
		case sf::Keyboard::Num7:
		case sf::Keyboard::Num8:
		case sf::Keyboard::Num9:
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
				BlendLayer(key.code - sf::Keyboard::Num1);
			break;

		case sf::Keyboard::Num0:
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
				BlendLayer(9);
			break;

		case sf::Keyboard::Space:
			// TODO: use proper target
            for (int i = 0; i < m_selection.Size(); ++i)
			    Actions::ActionPool::Instance().StartLocalAction(0, m_selection[i], 42);
			break;
		}
	}

	void MasterState::BlendLayer(int layerID)
	{
		if(m_firstLayerSelection)
		{
			int l;
			for(l = 0; l < 10; l++)
			{
				m_hiddenLayers[l] = 1;
			}
			m_firstLayerSelection = false;
		}
		// Toggle state: if number is pressed again hide layer
		m_hiddenLayers[layerID] = 1 - m_hiddenLayers[layerID];
	}

	void MasterState::OnBegin()
	{
		// Init server
		Network::Messenger::Initialize(nullptr);
	}

	void MasterState::OnEnd()
	{
		CommonState::OnEnd();

		// Save database

		// Save the world
		Jo::Files::HDDFile file(m_worldFileName, Jo::Files::HDDFile::OVERWRITE);
		g_Game->GetWorld()->Save( file );
	}


	void MasterState::Resize(const sf::Vector2f& _size)
	{
		// Scale chat too
		CommonState::Resize( _size );

		// Scale editor sidebar
		m_objectsPanel->setSize( m_objectsPanel->getSize().x, _size.y * 0.5f );
		m_modulePanel->setSize( m_modulePanel->getSize().x, _size.y * 0.5f );
		m_modulePanel->setPosition( 0.0f, _size.y * 0.5f );
		m_propertyPanel->setSize( m_propertyPanel->getSize().x, _size.y * 0.5f );
		m_viewPanel->setSize( m_viewPanel->getSize().x, _size.y * 0.5f );
		m_viewPanel->setPosition( m_modulePanel->getSize().x, _size.y * 0.5f );

		tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
		m_selectionView->setSize( m_selectionView->getSize().x, localOut->getPosition().y - 100.0f );
		m_selectionView->setPosition( _size.x - m_selectionView->getSize().x, 100.0f );

		m_toolbar->setSize( _size.x - m_objectsPanel->getSize().x*2, 100.0f );
	}


	void MasterState::MouseMoved(int deltaX, int deltaY, bool guiHandled)
	{
		CommonState::MouseMoved(deltaX, deltaY, guiHandled);

		// Don't react to any key if gui handled it
		if (guiHandled)
			return;

		if( sf::Mouse::isButtonPressed(sf::Mouse::Left) )
		{
			// Most actions here require the tile position -> compute it
			auto mousePos = sf::Mouse::getPosition( g_Game->GetWindow() );
			sf::Vector2f tilePos = g_Game->GetWindow().mapPixelToCoords(mousePos);
			tilePos.x /= TILESIZE;
			tilePos.y /= TILESIZE;
			int x = (int)floor(tilePos.x);
			int y = (int)floor(tilePos.y);

			// In brush mode paint to the current position
			if( m_modeTool->GetMode() == Interfaces::ModeToolbox::BRUSH )
			{
				m_brush.Paint(x, y);
			}
		}
	}

	Core::Map* MasterState::GetCurrentMap()
	{
		MapID id = m_mapTool->GetSelectedMap();
		return g_Game->GetWorld()->GetMap(id);
	}







	void MasterState::CreateDefaultPropertyBase()
	{
		Network::MaskWorldMessage lock;
		// Create and fill an object with all known properties
		ObjectID propertyOID = m_dbProperties->NewObject( STR_EMPTY );
		assert( propertyOID == 0 );
		Object* propertyO = m_dbProperties->GetObject( propertyOID );

		propertyO->SetColor( sf::Color::White );
		propertyO->Add( PROPERTY::NAME );
		propertyO->Add( PROPERTY::OBSTACLE );
		propertyO->Add( PROPERTY::INVENTORY );
		propertyO->Add( PROPERTY::STRENGTH );
		propertyO->Add( PROPERTY::DEXTERITY );
		propertyO->Add( PROPERTY::CONSTITUTION );
		propertyO->Add( PROPERTY::INTELLIGENCE );
		propertyO->Add( PROPERTY::WISDOM );
		propertyO->Add( PROPERTY::CHARISMA );
	}

	void MasterState::CreateDefaultModuleBase()
	{
		Network::MaskWorldMessage lock;
		ObjectID OID = m_dbModules->NewObject( STR_EMPTY );
		Object* object = m_dbModules->GetObject( OID );
		object->Add( PROPERTY::NAME ).SetValue( STR_ATTACKABLE );
		object->GetProperty( STR_PROP_SPRITE ).SetRights( Property::R_SYSTEMONLY );	// Hide the sprite property
		object->Add( PROPERTY::HEALTH );
	}

	void MasterState::CreateDefaultTemplateBase()
	{
		Network::MaskWorldMessage lock;
		ObjectID OID = m_dbTemplates->NewObject( "media/gobbo.png" );
		Object* object = m_dbTemplates->GetObject( OID );
		object->Add( PROPERTY::NAME ).SetValue( STR_GOBBO );
		// TODO: add modules

		OID = m_dbTemplates->NewObject( "media/bar_hor.png" );
		object = m_dbTemplates->GetObject( OID );
		object->Add( PROPERTY::NAME ).SetValue( STR_WALLH );
		object->Add( PROPERTY::OBSTACLE );
		OID = m_dbTemplates->NewObject( "media/bar_vert.png" );
		object = m_dbTemplates->GetObject( OID );
		object->Add( PROPERTY::NAME ).SetValue( STR_WALLV );
		object->Add( PROPERTY::OBSTACLE );
		OID = m_dbTemplates->NewObject( "media/cross_big.png" );
		object = m_dbTemplates->GetObject( OID );
		object->Add( PROPERTY::NAME ).SetValue( STR_WALLC );
		object->Add( PROPERTY::OBSTACLE );
	}


}// namespace States
