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
#include "Game.hpp"
#include "states/StateMachine.hpp"
#include "sfutils/View.hpp"
#include "utils/StringUtil.hpp"
#include "gamerules/Combat.hpp"
#include "states/PromptState.hpp"


States::PlayerState::PlayerState( const std::string& _playerName, const sf::Color& _chatColor, Core::PlayerID _id ) :
	m_player(nullptr),
	m_playerView(nullptr),
	m_focus(nullptr)
{
	m_color = _chatColor;
	m_name = _playerName;
	m_playerID = _id;

	m_playerView = Interfaces::PropertyPanel::Ptr(m_gui);
}


void States::PlayerState::Draw(sf::RenderWindow& win)
{
	assert( m_focus );

	// Draw some color to the background
	static sf::Color c(20, 26, 36);
	win.clear(c);

	// Focus on the player
	if( m_focus->IsLocatedOnAMap() )
	{
		sf::Vector2f pos = m_focus->GetPosition();
		sf::Vector2f viewPos = pos * float(TILESIZE);
		viewPos.x += m_playerView->getSize().x * 0.5f;
		sf::View newView = win.getView();
		newView.setCenter(viewPos);
		win.setView(newView);

		// Render
		using namespace std::placeholders;
		std::function<float(Core::Map&,sf::Vector2i&)> visibilityFunc =
			std::bind(&PlayerState::CheckTileVisibility, this, _1, _2, m_focus->GetPosition());
		Graphics::TileRenderer::Render(win, *GetCurrentMap(),
			visibilityFunc);

		// Draw the players path
		DrawPathOverlay(win, m_focus);

		Graphics::TileRenderer::RenderSelection( win, m_selection );
	} else {
		// The player is not on the map - bring that into attention
		sf::View backup = sfUtils::View::SetDefault(&win);
		sf::Text t(STR_PLAYER_NOT_ON_MAP, m_gui.getGlobalFont(), 30);
		t.setPosition(230, 320);
		win.draw(t);
		win.setView(backup);
	}

	// Draw the current focused object's name
	sf::View backup = sfUtils::View::SetDefault(&win);
	sf::Text t(m_focus->GetName(), m_gui.getGlobalFont(), 24);
	t.setPosition(20, 10);
	win.draw(t);
	win.setView(backup);

	GameState::Draw(win);
}


void States::PlayerState::MouseMoved(int deltaX, int deltaY, bool guiHandled)
{
    // TODO: update default action!

	// Don't react to any key if gui handled it
	if (guiHandled)
		return;
}


void States::PlayerState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled)
{
	// Return if the GUI already handled it
	if (guiHandled)
		return;

	// Don't interact if focused object cannot be controlled
	if( !m_focus->IsLocatedOnAMap() ||
		m_focus->GetProperty( STR_PROP_OWNER ).Value() != m_player->GetName() )
		return;

	int tileX = (int)tilePos.x;
	int tileY = (int)tilePos.y;

	switch (button.button)
	{
	case sf::Mouse::Left: {
		//------------------------------------//
		// move player to tile position		  //
		//------------------------------------//
		assert(m_focus->IsLocatedOnAMap());
		auto& tiles = GetCurrentMap()->GetObjectsAt(tileX,tileY);
		if( tiles.Size() > 0 )
		{
			Core::Object* object = g_Game->GetWorld()->GetObject(tiles[tiles.Size()-1]);
			Core::ObjectList focusList;
			focusList.Add(m_focus->ID());
			Actions::ActionPool::Instance().UpdateDefaultAction(focusList, object);
			Actions::ActionPool::Instance().StartDefaultAction(m_focus->ID(), object->ID());

			// Delete current target(s) if not appending
			//if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
			//{
			//	m_focus->GetProperty(STR_PROP_TARGET).SetValue(STR_EMPTY);
			//	Core::Property& path = m_player->GetProperty(STR_PROP_PATH);
			//	path.ClearObjects();
			//	path.SetValue(STR_FALSE);
			//}
			//// Append to target list
			//m_focus->AppendToPath( obj->ID() );
		}
		break; }

	case sf::Mouse::Right: {
		if( GetCurrentMap()->GetObjectsAt(tileX,tileY).Size() > 0 )
		{
			SelectionState* gs = dynamic_cast<SelectionState*>(g_Game->GetStateMachine()->PushGameState(GST_SELECTION));
			gs->SetTilePosition(tileX,tileY);
		}
		break; }
	}
}


void States::PlayerState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled)
{
	CommonState::MouseWheelMoved(wheel, guiHandled);

	// Manually scroll panels (tgui::callback not provided for this action).
	// The scroll call has an effect only if the mouse is on the respective
	// element. So calling it for everybody is just fine.
	m_playerView->Scroll(wheel.delta);
}


void States::PlayerState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
	switch(key.code) //pre gui switch to still get event with LControl
	{
	case sf::Keyboard::Num1:
	case sf::Keyboard::Num2:
	case sf::Keyboard::Num3:
	case sf::Keyboard::Num4:
	case sf::Keyboard::Num5:
	case sf::Keyboard::Num6:
	case sf::Keyboard::Num7:
	case sf::Keyboard::Num8:
	case sf::Keyboard::Num9:
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
			if(m_focus!=nullptr)//check for null
				SetHotkeyToObject(key.code - sf::Keyboard::Num1, m_focus->ID());
		}
		else
			SetViewToHotkey(key.code - sf::Keyboard::Num1);
		break;
	}

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
	case sf::Keyboard::LAlt:
		m_focus = m_player;
		break;
	case sf::Keyboard::Tab: {
		Core::Object* object;
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::LShift ) )
			object = g_Game->GetWorld()->GetNextObservableObject(m_focus->ID(), -1);
		else
			object = g_Game->GetWorld()->GetNextObservableObject(m_focus->ID(), 1);
		if(object != nullptr)
		{
			m_focus = object;
			// Always have the current focused element in selection
			m_selection.Clear();
			m_selection.Add( object->ID() );
		}
		break; }
	}
}



void States::PlayerState::OnBegin()
{
	// The client is receiving the world after connecting
	Network::Messenger::Poll( true );

	// The player name is used to find the correct object in the world.
	// TODO: if player does not exists create one!
	m_player = g_Game->GetWorld()->FindPlayer( m_name );
	assert( m_player );
	m_player->GetProperty(STR_PROP_PLAYER).SetValue( std::to_string(m_playerID) );
	// Use the players currently chosen color
	m_player->SetColor( m_color );

	m_focus = m_player;

	tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
	m_playerView->Init( 624.0f, 0.0f, 400.0f, localOut->getPosition().y, false, false,
		m_playerID, nullptr );
	m_playerView->Show( g_Game->GetWorld(), m_player );

	// Auto select the player (he is the actor)
	this->AddToSelection(m_player->ID());
}


void States::PlayerState::Update( float _dt )
{
	CommonState::Update( _dt );

	// Go through player object and update all viewed properties
	m_playerView->Show( g_Game->GetWorld(), m_player );
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
	return g_Game->GetWorld()->GetMap(m_focus->GetParentMap());
}


float States::PlayerState::CheckTileVisibility( Core::Map& _map, sf::Vector2i& _tilePos, sf::Vector2f& _playerPos ) const
{
	sf::Vector2f direction = sf::Vector2f(_tilePos) - _playerPos;
	float distance = sfUtils::Length(direction);

	// The father away the less visible
	float v = std::max(0.0f, std::min( 1.0f, 2.0f - distance * 0.3f ));

	// Search an occluder - simple ray march with a Bresenham.
	// http://de.wikipedia.org/wiki/Bresenham-Algorithmus#C-Implementierung
#ifndef _DEBUG	// Too slow in debug mode.
	direction = direction / distance;
	sf::Vector2i currentTile = sfUtils::Round(_playerPos);
	int dx =  abs(_tilePos.x-currentTile.x), sx = currentTile.x<_tilePos.x ? 1 : -1;
	int dy = -abs(_tilePos.y-currentTile.y), sy = currentTile.y<_tilePos.y ? 1 : -1;
	int err = dx+dy, e2; // error value e_xy
	while(v > 0.001f && currentTile != _tilePos)
	{
		if( !_map.IsFree(currentTile) )
		{
			// Use ray - tile distance to weight the occlusion.
		//	sf::Vector2f closest = _playerPos + sfUtils::Dot(sf::Vector2f(currentTile) - _playerPos, direction) * direction;
		//	v *= std::min(1.0f, std::max(0.0f, sfUtils::Length(closest - sf::Vector2f(currentTile)) - 0.2f));
			v = 0.0f;
		}
		e2 = 2*err;
		if (e2 > dy) { err += dy; currentTile.x += sx; }	// e_xy+e_x > 0
		if (e2 < dx) { err += dx; currentTile.y += sy; }	// e_xy+e_y < 0
	}//*/

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
	}//*/
#endif

	return v;
}

void States::PlayerState::SetViewToHotkey(const int hotkey){
	if (m_hotkeys.find(hotkey) == m_hotkeys.end())//return if there is no hotkey
		return;
	SetViewToObject(g_Game->GetWorld()->GetObject(m_hotkeys[hotkey]));
}

void States::PlayerState::SetViewToObject(Core::Object* object){
	m_focus=object;
	if(Utils::IStringEqual(object->GetProperty(STR_PROP_OWNER).Value(),m_name))
	{
		m_selection.Clear();
		AddToSelection(object->ID());
	}
	else
	{
		m_selection.Clear();
		AddToSelection(m_player->ID());
	}
}

void States::PlayerState::SetHotkeyToObject(const int hotkey, Core::ObjectID objectID){
	m_hotkeys[hotkey] = objectID;
}

void States::PlayerState::BeginCombat( Core::ObjectID _object )
{
	// Find the object
	Core::Object* object = g_Game->GetWorld()->GetObject(_object);

	// Does this object belong to me?
	if (object->HasProperty(STR_PROP_OWNER) && object->GetProperty(STR_PROP_OWNER).Value() == m_name)
	{
		// Maybe create a new Combat object
		if (!m_combat)
			m_combat = new GameRules::Combat();

		// Prompt for initiative roll
		m_combat->PushInitiativePrompt(_object);
	}
}
