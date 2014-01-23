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
#include <iostream>
#include "core/PredefinedProperties.hpp"

using namespace Core;

namespace States {

	MasterState::MasterState( const std::string& _loadFile ) :
		m_player(nullptr),
		m_propertyPanel(nullptr),
		m_dbProperties(nullptr),
		m_dbModules(nullptr),
		m_dbTemplates(nullptr),
		m_modulePanel(nullptr),
		m_objectsPanel(nullptr),
		m_selectionView(nullptr),
		m_toolbar(nullptr)
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
		for( int i=0; i<5; ++i )
		m_toolbar->AddToolbox( Interfaces::MapToolbox::Ptr() );

		// Set chat color...
		m_color = sf::Color(80,80,250);
		m_name = "Master";
	}

	void MasterState::Update(float dt)
	{
		CommonState::Update(dt);
		// Uses the test map 0 for testing purposes.
		g_Game->GetWorld()->GetMap(0)->Update(dt);

		m_toolbar->Update( dt );

		if( m_selectionChanged )
		{
			if( m_selection.Size() > 0 )
			{
				m_selectionView->show();
			} else m_selectionView->hide();
			m_selectionChanged = false;
		}

		if( m_selectionView->isVisible() )
			m_selectionView->Show( g_Game->GetWorld(), m_selection );
	}

	void MasterState::Draw(sf::RenderWindow& win)
	{
		// Hard coded test selection:
		if( !m_selected ) {
			m_selected = g_Game->GetWorld()->GetObject(g_Game->GetWorld()->GetMap(0)->GetObjectsAt(0,2)[1]);
			m_player = m_selected;
		}

		// Draw some color to the background
		static sf::Color c(20, 26, 36);
		win.clear(c);

		// Render
		// Uses the test map 0 for testing purposes.
		Graphics::TileRenderer::Render(win, *g_Game->GetWorld()->GetMap(0),
			[](Core::Map&,sf::Vector2i&){ return 1.0f; });

		// If the selected object has a path draw it
		DrawPathOverlay(win, m_player);

		Graphics::TileRenderer::RenderSelection( win, m_selection );

		GameState::Draw(win);
	}

	void MasterState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled)
	{

		// Return if the GUI already handled it
		if (guiHandled)
			return; 

		int tileX = (int)floor(tilePos.x);
		int tileY = (int)floor(tilePos.y);
		switch (button.button)
		{
		case sf::Mouse::Left: {
			//------------------------------------//
			// move player to tile position		  //
			//------------------------------------//
			auto& tiles = g_Game->GetWorld()->GetMap(0)->GetObjectsAt(tileX, tileY);
			if( tiles.Size() > 0 )
			{
				// TODO: intelligent select?
				m_selected = g_Game->GetWorld()->GetObject(tiles[tiles.Size()-1]);
				// Delete current target(s) if not appending
				if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
				{
					m_player->GetProperty(STR_PROP_TARGET).SetValue(STR_EMPTY);
					if( !m_player->HasProperty(STR_PROP_PATH) )
						m_player->Add( PROPERTY::PATH );
					Property& path = m_player->GetProperty(STR_PROP_PATH);
					path.ClearObjects();
					path.SetValue(STR_FALSE);
				}
				if(m_selected->GetProperty(STR_PROP_LAYER).Value()==STR_0){
					// Append to target list
					m_player->AppendToPath( m_selected->ID() );
				}
			}
			} break;
		case sf::Mouse::Right: {
			if( g_Game->GetWorld()->GetMap(0)->GetObjectsAt(tileX, tileY).Size() > 0 )
			{
				SelectionState* gs = dynamic_cast<SelectionState*>(g_Game->GetStateMachine()->PushGameState(GST_SELECTION));
				gs->SetTilePosition(tileX, tileY);
			}
			
			} break;
		}
	}


	void MasterState::OnBegin()
	{
		// Init server
		Network::Messenger::Initialize(nullptr);
	}

	void MasterState::OnEnd()
	{
		CommonState::OnEnd();
	}

	void MasterState::MouseButtonReleased( sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, float time, bool guiHandled)
	{
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
			} else {	// Things where dragged to the map.
				if( m_draggedContent->from == Interfaces::DragContent::OBJECT_PANEL )
				{
					// Insert object copy to the map
					ObjectID id = g_Game->GetWorld()->NewObject( m_draggedContent->object );

					int x = (int)floor(tilePos.x);
					int y = (int)floor(tilePos.y);
					// Meaningful layer: on top
					int l = g_Game->GetWorld()->GetMap(0)->GetObjectsAt(x,y).Size();
					g_Game->GetWorld()->GetMap(0)->Add( id, x, y, l );
				}
			}

			delete m_draggedContent;
			m_draggedContent = nullptr;
		}
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
		m_selectionView->setSize( m_selectionView->getSize().x, localOut->getPosition().y );
		m_selectionView->setPosition( _size.x - m_selectionView->getSize().x, 0.0f );
	}












	void MasterState::CreateDefaultPropertyBase()
	{
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
		ObjectID OID = m_dbModules->NewObject( STR_EMPTY );
		Object* object = m_dbModules->GetObject( OID );
		object->Add( PROPERTY::NAME ).SetValue( STR_ATTACKABLE );
		object->GetProperty( STR_PROP_SPRITE ).SetRights( Property::R_SYSTEMONLY );	// Hide the sprite property
		object->Add( PROPERTY::HEALTH );
	}

	void MasterState::CreateDefaultTemplateBase()
	{
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
