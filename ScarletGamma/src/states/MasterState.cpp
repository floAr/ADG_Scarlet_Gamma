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

using namespace Core;

namespace States {

	MasterState::MasterState( const std::string& _loadFile ) :
		m_player(nullptr),
		m_propertyPanel(nullptr),
		m_dbProperties(nullptr),
		m_dbModules(nullptr),
		m_dbTemplates(nullptr)
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
		m_viewPanel->Init( 240.0f, 300.0f, 240.0f, 300.0f, false, false, 0, &m_draggedContent );

		// Load properties from database into gui
		m_propertyPanel = Interfaces::PropertyPanel::Ptr(m_gui);
		m_propertyPanel->Init( 0.0f, 0.0f, 240.0f, 300.0f, true, false, 0, &m_draggedContent );
		// The all-properties object has ID 0
		m_propertyPanel->Show( m_dbProperties->GetObject( 0 ) );

		// Load Modules from database into gui
		m_modulePanel = Interfaces::ObjectPanel::Ptr(m_gui);
		m_modulePanel->Init( 0.0f, 300.0f, 240.0f, 300.0f, true, m_dbModules, Interfaces::DragContent::MODULES_PANEL, &m_draggedContent, m_viewPanel );

		// Load Objects from database into gui
		m_objectsPanel = Interfaces::ObjectPanel::Ptr(m_gui);
		m_objectsPanel->Init( 240.0f, 0.0f, 240.0f, 300.0f, true, m_dbTemplates, Interfaces::DragContent::OBJECT_PANEL, &m_draggedContent, m_viewPanel );

		// Set chat color...
		m_color = sf::Color(80,80,250);
		m_name = "Master";
	}

	void MasterState::Update(float dt)
	{
		CommonState::Update(dt);
		// Uses the test map 0 for testing purposes.
		g_Game->GetWorld()->GetMap(0)->Update(dt);
		//std::cout<<m_selection.Size();
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
		Graphics::TileRenderer::Render(win, *g_Game->GetWorld()->GetMap(0));

		// If the selected object has a path draw it
		DrawPathOverlay(win, m_player);

		GameState::Draw(win);
	}

	void MasterState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos)
	{
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
					m_player->GetProperty(Core::Object::PROP_TARGET).SetValue("");
					Core::Property& path = m_player->GetProperty(Core::Object::PROP_PATH);
					path.ClearObjects();
					path.SetValue("false");
				}
				if(m_selected->GetProperty(Core::Object::PROP_LAYER).Value()=="0"){
					// Append to target list
					m_player->AppendToPath( m_selected->ID() );
				}
			}
			} break;
		case sf::Mouse::Middle: {
			m_zoom = 0;
			sf::RenderWindow& win = g_Game->GetWindow();
			sf::View newView = win.getView();
			newView.setSize((float)win.getSize().x, (float)win.getSize().y);
			win.setView(newView);
			} break;
		case sf::Mouse::Right: {
			if( g_Game->GetWorld()->GetMap(0)->GetObjectsAt(tileX, tileY).Size() > 0 )
			{
				SelectionState* gs = dynamic_cast<SelectionState*>(g_Game->GetStateMachine()->PushGameState(GST_SELECTION));
				gs->SetTilePosition(tileX, tileY, (float)button.x, (float)button.y);
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

	void MasterState::MouseButtonReleased( sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, float time )
	{
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
	}












	void MasterState::CreateDefaultPropertyBase()
	{
		// Create and fill an object with all known properties
		ObjectID propertyOID = m_dbProperties->NewObject( STR_EMPTY );
		assert( propertyOID == 0 );
		Object* propertyO = m_dbProperties->GetObject( propertyOID );

		propertyO->SetColor( sf::Color::White );
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, Object::PROP_NAME, STR_EMPTY ));
		propertyO->Add( Property(propertyOID, Property::R_VC0000000, Object::PROP_OBSTACLE, STR_EMPTY ));
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_INVENTORY, STR_EMPTY, ObjectList() ));
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_STRENGTH, STR_0 ));
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_DEXTERITY, STR_0 ));
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_CONSTITUTION, STR_0 ));
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_INTELLIGENCE, STR_0 ));
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_WISDOM, STR_0 ));
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_CHARISMA, STR_0 ));
	}

	void MasterState::CreateDefaultModuleBase()
	{
		ObjectID OID = m_dbModules->NewObject( STR_EMPTY );
		Object* object = m_dbModules->GetObject( OID );
		object->Add( Property(OID, Property::R_SYSTEMONLY, Object::PROP_NAME, STR_ATTACKABLE ));
		object->Add( Property(OID, Property::R_SYSTEMONLY, Object::PROP_SPRITE, STR_EMPTY ));	// Hide the sprite property
		object->Add( Property(OID, Property::R_VCEV00000, STR_HEALTH, STR_0 ));
	}

	void MasterState::CreateDefaultTemplateBase()
	{
		ObjectID OID = m_dbTemplates->NewObject( "media/gobbo.png" );
		Object* object = m_dbTemplates->GetObject( OID );
		object->Add( Property(OID, Property::R_V0E000000, Object::PROP_NAME, STR_GOBBO ));
		// TODO: add modules

		OID = m_dbTemplates->NewObject( "media/bar_hor.png" );
		object = m_dbTemplates->GetObject( OID );
		object->Add( Property(OID, Property::R_V0E000000, Object::PROP_NAME, STR_WALLH ));
		object->Add( Property(OID, Property::R_VC0000000, Object::PROP_OBSTACLE, STR_EMPTY ));
		OID = m_dbTemplates->NewObject( "media/bar_vert.png" );
		object = m_dbTemplates->GetObject( OID );
		object->Add( Property(OID, Property::R_V0E000000, Object::PROP_NAME, STR_WALLV ));
		object->Add( Property(OID, Property::R_VC0000000, Object::PROP_OBSTACLE, STR_EMPTY ));
		OID = m_dbTemplates->NewObject( "media/cross_big.png" );
		object = m_dbTemplates->GetObject( OID );
		object->Add( Property(OID, Property::R_V0E000000, Object::PROP_NAME, STR_WALLC ));
		object->Add( Property(OID, Property::R_VC0000000, Object::PROP_OBSTACLE, STR_EMPTY ));
	}

}// namespace States
