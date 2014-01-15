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

namespace States {

	MasterState::MasterState( const std::string& _loadFile ) :
		m_player(nullptr)
	{
		// Load the map
		Jo::Files::HDDFile file(_loadFile);
		g_Game->GetWorld()->Load( file );

		// Set chat color...
		m_color = sf::Color(80,80,250);
		m_name = "[Master] ";


	}

	void MasterState::Update(float dt)
	{
		CommonState::Update(dt);
		// Uses the test map 0 for testing purposes.
		g_Game->GetWorld()->GetMap(0)->Update(dt);
		std::cout<<m_selection.Size();
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

}// namespace States
