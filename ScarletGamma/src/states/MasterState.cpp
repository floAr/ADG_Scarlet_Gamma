#include "states/MasterState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "states/StateMachine.hpp"
#include "Constants.hpp"
#include "utils/Falloff.hpp"
#include "network/Messenger.hpp"
#include <iostream>

using namespace Core;

namespace States {

	MasterState::MasterState( const std::string& _loadFile ) :
		m_player(nullptr),
		m_propertyPanel(nullptr),
		m_database(nullptr)
	{
		// Load the map
		Jo::Files::HDDFile file(_loadFile);
		g_Game->GetWorld()->Load( file );

		// Load the template database
		m_database = new Core::World();
		if( !Jo::Files::Utils::Exists( "data/templates.dat" ) )
			CreateDefaultDatabase();
		else {
			Jo::Files::HDDFile database( "data/templates.dat" );
			m_database->Load( database );
		}

		// Load properties from database into gui
		m_propertyPanel = Interfaces::PropertyPanel::Ptr(m_gui);
		m_propertyPanel->Init( 0.0f, 0.0f, 240.0f, 300.0f, true, false, 0 );
		m_propertyObject = m_database->GetObject( 0 );	// The all-properties object has ID 0
		m_propertyPanel->Show( m_propertyObject );

		// Set chat color...
		m_color = sf::Color(80,80,250);
		m_name = "[Master] ";
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
		switch (button.button)
		{
		case sf::Mouse::Left: {
			//------------------------------------//
			// move player to tile position		  //
			//------------------------------------//
			auto& tiles = g_Game->GetWorld()->GetMap(0)->GetObjectsAt((int)tilePos.x,(int)tilePos.y);
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
		case sf::Mouse::Middle:{
			m_zoom = 0;
			sf::RenderWindow& win = g_Game->GetWindow();
			sf::View newView = win.getView();
			newView.setSize((float)win.getSize().x, (float)win.getSize().y);
			win.setView(newView);
							   } break;
		case sf::Mouse::Right:{
			SelectionState* gs = dynamic_cast<SelectionState*>(g_Game->GetStateMachine()->PushGameState(GST_SELECTION));
			gs->AddTilePosition((int)tilePos.x,(int)tilePos.y);
			
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












	void MasterState::CreateDefaultDatabase()
	{
		// Create and fill an object with all known properties
		ObjectID propertyOID = m_database->NewObject( STR_EMPTY );
		assert( propertyOID == 0 );
		Object* propertyO = m_database->GetObject( propertyOID );

		propertyO->SetColor( sf::Color::White );
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, Object::PROP_NAME, STR_EMPTY ) );
		propertyO->Add( Property(propertyOID, Property::R_VC0000000, Object::PROP_OBSTACLE, STR_EMPTY ) );
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_INVENTORY, STR_EMPTY, ObjectList() ) );
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_STRENGTH, STR_EMPTY ) );
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_DEXTERITY, STR_EMPTY ) );
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_CONSTITUTION, STR_EMPTY ) );
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_INTELLIGENCE, STR_EMPTY ) );
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_WISDOM, STR_EMPTY ) );
		propertyO->Add( Property(propertyOID, Property::R_VCEV0EV00, STR_CHARISMA, STR_EMPTY ) );
	}

}// namespace States
