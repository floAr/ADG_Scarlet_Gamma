#include "states/PlayerState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "utils/Falloff.hpp"
#include "network/Messenger.hpp"
#include <iostream>


States::PlayerState::PlayerState( const std::string& _playerName, const sf::Color& _chatColor ) :
	m_player(nullptr),
	m_playerView(nullptr)
{
	m_color = _chatColor;
	m_name = _playerName;

	m_playerView = Interfaces::PropertyPanel::Ptr(m_gui);
}


void States::PlayerState::Draw(sf::RenderWindow& win)
{
	// Draw some color to the background
	static sf::Color c(20, 26, 36);
	win.clear(c);

	// Render
	// Uses the test map 0 for testing purposes.
	assert(m_player->IsLocatedOnAMap());
	Graphics::TileRenderer::Render(win, *g_Game->GetWorld()->GetMap(m_player->GetParentMap()));

	// Draw the players path
	DrawPathOverlay(win, m_player);

	GameState::Draw(win);
}

void States::PlayerState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos)
{
	switch (button.button)
	{
	case sf::Mouse::Left: {
		//------------------------------------//
		// move player to tile position		  //
		//------------------------------------//
		assert(m_player->IsLocatedOnAMap());
		auto& tiles = g_Game->GetWorld()->GetMap(m_player->GetParentMap())->GetObjectsAt((int)tilePos.x,(int)tilePos.y);
		if( tiles.Size() > 0 )
		{
			// TODO: intelligent select?
			m_selected = g_Game->GetWorld()->GetObject(tiles[tiles.Size()-1]);
			// Delete current target(s) if not appending
			if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
			{
				m_player->GetProperty(Core::Object::PROP_TARGET).SetValue(STR_EMPTY);
				Core::Property& path = m_player->GetProperty(Core::Object::PROP_PATH);
				path.ClearObjects();
				path.SetValue(STR_FALSE);
			}
			if(m_selected->GetProperty(Core::Object::PROP_LAYER).Value()=="0"){
				// Append to target list
				m_player->AppendToPath( m_selected->ID() );
			}
		}
	} break;
	case sf::Mouse::Middle:
		m_zoom = 0;
		sf::RenderWindow& win = g_Game->GetWindow();
		sf::View newView = win.getView();
		newView.setSize((float)win.getSize().x, (float)win.getSize().y);
		win.setView(newView);
		break;
	}
}



void States::PlayerState::OnBegin()
{
	// After connecting the client is receiving the world
	Network::Messenger::Poll( true );

	// The player name is used to find the correct object in the world.
	// TODO: if player does not exists create one!
	m_player = g_Game->GetWorld()->FindPlayer( m_name );
	assert( m_player );
	Core::PlayerID id = m_player->GetProperty(Core::Object::PROP_PLAYER).Evaluate();
	// Use the players currently chosen color
	m_player->SetColor( m_color );
	m_player->GetProperty( Core::Object::PROP_COLOR ).ApplyRights( 
		id, true );

	m_playerView->Init( 0.0f, 0.0f, 300.0f, 600.0f, false, false,
		id, nullptr );
	m_playerView->Show( m_player );
}

void States::PlayerState::Update( float _dt )
{
	CommonState::Update( _dt );

	// Go through player object and update all viewed properties
	m_playerView->Show(m_player);
}
