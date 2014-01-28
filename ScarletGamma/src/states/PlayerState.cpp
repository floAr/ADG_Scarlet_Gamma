#include "states/PlayerState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "utils/Falloff.hpp"
#include "network/Messenger.hpp"
#include "actions/ActionPool.hpp"
#include <iostream>


States::PlayerState::PlayerState( const std::string& _playerName, const sf::Color& _chatColor ) :
	m_player(nullptr),
	m_playerView(nullptr),
	m_focus(nullptr)
{
	m_color = _chatColor;
	m_name = _playerName;

	m_playerView = Interfaces::PropertyPanel::Ptr(m_gui);
}


void States::PlayerState::Draw(sf::RenderWindow& win)
{
	// Focus on the player
	if( m_focus )
	{
		sf::Vector2f pos = m_focus->GetPosition();
		sf::Vector2f viewPos = pos * float(TILESIZE);
		viewPos.x += m_playerView->getSize().x * 0.5f;
		sf::View newView = win.getView();
		newView.setCenter(viewPos);
		win.setView(newView);
	}

	// Draw some color to the background
	static sf::Color c(20, 26, 36);
	win.clear(c);

	// Render
	assert(m_player->IsLocatedOnAMap());
	using namespace std::placeholders;
	std::function<float(Core::Map&,sf::Vector2i&)> visibilityFunc =
		std::bind(&PlayerState::CheckTileVisibility, this, _1, _2, m_player->GetPosition());
	Graphics::TileRenderer::Render(win, *GetCurrentMap(),
		visibilityFunc);

	// Draw the players path
	DrawPathOverlay(win, m_player);

	Graphics::TileRenderer::RenderSelection( win, m_selection );

	GameState::Draw(win);
}


void States::PlayerState::MouseMoved(int deltaX, int deltaY, bool guiHandled)
{
	// Don't react to any key if gui handled it
	if (guiHandled)
		return;

	// Let the common state do the most things
	CommonState::MouseMoved(deltaX, deltaY, guiHandled);

	// In the case of mbm down the view was moved - disable focusing
	if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
		m_focus = nullptr;
}


void States::PlayerState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled)
{
	// Return if the GUI already handled it
	if (guiHandled)
		return;

	switch (button.button)
	{
	case sf::Mouse::Left: {
		//------------------------------------//
		// move player to tile position		  //
		//------------------------------------//
		assert(m_player->IsLocatedOnAMap());
		auto& tiles = GetCurrentMap()->GetObjectsAt((int)tilePos.x,(int)tilePos.y);
		if( tiles.Size() > 0 )
		{
			// TODO: intelligent select?
			Core::Object* obj = g_Game->GetWorld()->GetObject(tiles[tiles.Size()-1]);
			// Delete current target(s) if not appending
			if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
			{
				m_player->GetProperty(STR_PROP_TARGET).SetValue(STR_EMPTY);
				Core::Property& path = m_player->GetProperty(STR_PROP_PATH);
				path.ClearObjects();
				path.SetValue(STR_FALSE);
			}
			if(obj->GetProperty(STR_PROP_LAYER).Value()=="0"){
				// Append to target list
				m_player->AppendToPath( obj->ID() );
			}
		}
		break; }
	}
}


void States::PlayerState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
	// Don't react to any key if gui handled it
	if (guiHandled)
		return;

    // Let common state handle input
    CommonState::KeyPressed(key, guiHandled);

	switch(key.code)
	{
	case sf::Keyboard::Num0:
	case sf::Keyboard::Numpad0:
		// Refocus on player
		m_focus = m_player;
		break;
    case sf::Keyboard::Space:
        // TODO: use selection as target
        Actions::ActionPool::Instance().StartLocalAction(0, m_player->ID(), 42);
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
	Core::PlayerID id = m_player->GetProperty(STR_PROP_PLAYER).Evaluate();
	// Use the players currently chosen color
	m_player->SetColor( m_color );
	m_player->GetProperty( STR_PROP_COLOR ).ApplyRights( 
		id, true );

	m_focus = m_player;

	tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
	m_playerView->Init( 624.0f, 0.0f, 400.0f, localOut->getPosition().y, false, false,
		id, nullptr );
	m_playerView->Show( m_player );
}


void States::PlayerState::Update( float _dt )
{
	CommonState::Update( _dt );

	// Go through player object and update all viewed properties
	m_playerView->Show(m_player);
}


void States::PlayerState::Resize(const sf::Vector2f& _size)
{
	// Scale chat too
	CommonState::Resize( _size );

	tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );

	// Scale player view
	m_playerView->setSize( m_playerView->getSize().x, localOut->getPosition().y );
	m_playerView->setPosition( _size.x - m_playerView->getSize().x, 0.0f );
}


Core::Map* States::PlayerState::GetCurrentMap()
{
	return g_Game->GetWorld()->GetMap(m_player->GetParentMap());
}


float States::PlayerState::CheckTileVisibility( Core::Map& _map, sf::Vector2i& _tilePos, sf::Vector2f& _playerPos ) const
{
	sf::Vector2f direction = sf::Vector2f(_tilePos) - _playerPos;
	float distance = sfUtils::Length(direction);

	// The father away the less visible
	float v = std::min( 1.0f, 2.0f - distance * 0.3f );

	// Search an occluder - simple ray march with a Bresenham.
	// http://de.wikipedia.org/wiki/Bresenham-Algorithmus#C-Implementierung
#ifndef _DEBUG	// Too slow in debug mode.
	direction = direction / distance;
	sf::Vector2i currentTile = sfUtils::Round(_playerPos);
	int dx =  abs(_tilePos.x-currentTile.x), sx = currentTile.x<_tilePos.x ? 1 : -1;
	int dy = -abs(_tilePos.y-currentTile.y), sy = currentTile.y<_tilePos.y ? 1 : -1;
	int err = dx+dy, e2; /* error value e_xy */
	while(currentTile != _tilePos)
	{
		if( !_map.IsFree(currentTile) )
		{
			// Use ray - tile distance to weight the occlusion.
			sf::Vector2f closest = _playerPos + sfUtils::Dot(sf::Vector2f(currentTile) - _playerPos, direction) * direction;
			v *= std::min(1.0f, sfUtils::Length(closest - sf::Vector2f(currentTile)));
		}
		e2 = 2*err;
		// Modification: only go in one direction (4 neighborhood) in one step.
		if (e2 > dy) { err += dy; currentTile.x += sx; } /* e_xy+e_x > 0 */
		else if (e2 < dx) { err += dx; currentTile.y += sy; } /* e_xy+e_y < 0 */
	}
#endif

	/*float t = 1.0f;
	while( t < distance && v > 0.01f )
	{
		sf::Vector2f rayPos = _playerPos + direction * t;
		sf::Vector2i tile = sfUtils::Round(rayPos);
		if( tile == _tilePos ) return v;
		if( !_map.IsFree(tile) )
		{
			// Use ray - tile distance to weight the occlusion.
			sf::Vector2f closest = _playerPos + sfUtils::Dot(sf::Vector2f(tile) - _playerPos, direction) * direction;
			v *= std::min(1.0f, sfUtils::Length(closest - sf::Vector2f(tile)));
		}
			//v *= std::min(1.0f, sfUtils::Length(sf::Vector2f(tile) - rayPos));
		t += 0.5f;
	}*/

	return v;
}
