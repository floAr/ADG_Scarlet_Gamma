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
#include "NewPlayerState.hpp"
#include "utils/ValueInterpreter.hpp"
#include "utils/Exception.hpp"


States::PlayerState::PlayerState( Core::ObjectID _player ) :
	m_player(nullptr),
	m_playerView(nullptr),
	m_observerView(nullptr),
	m_focus(nullptr)
{
	m_playerObjectID = _player;
	m_player = g_Game->GetWorld()->GetObject(_player);
	m_playerID = m_player->GetProperty(STR_PROP_PLAYER).Evaluate();
	m_color = m_player->GetColor();
	m_name = m_player->GetName();

	// Set camera at the player's position
	m_focus = m_player;

	m_playerView = Interfaces::PropertyPanel::Ptr(m_gui);

	m_observerView = Interfaces::PropertyPanel::Ptr(m_gui);
	m_observerView->Init( 0.0f, 0.0f, 300.0f, (float)g_Game->GetWindow().getSize().y, false, false,
		m_playerID, &m_draggedContent );
	m_observerView->hide();
}


void States::PlayerState::Draw(sf::RenderWindow& win)
{
	assert( m_focus );

	// Draw some color to the background
	static sf::Color c(20, 26, 36);
	win.clear(c);

	if( g_Game->GetWorld()->IsPaused() )
	{
		SetGuiView();
		sf::Text t(STR_PAUSE, m_gui.getGlobalFont(), 30);
		t.setPosition(30, 320);
		win.draw(t);
	} else {
		// Focus on the player
		if( m_focus->IsLocatedOnAMap() )
		{
			GoTo( m_focus );

			// Render
			using namespace std::placeholders;
			std::function<float(Core::Map&,sf::Vector2i&)> visibilityFunc =
				std::bind(&PlayerState::CheckTileVisibility, this, _1, _2, m_focus->GetPosition());
			Graphics::TileRenderer::Render(win, *GetCurrentMap(), visibilityFunc,
				(const bool*)(&m_hiddenLayers[0]));

			// Draw the players path
			DrawPathOverlay(win, m_focus);

			Graphics::TileRenderer::RenderSelection( win, m_selection );
		} else {
			// The player is not on the map - bring that into attention
			SetGuiView();
			sf::Text t(STR_PLAYER_NOT_ON_MAP, m_gui.getGlobalFont(), 30);
			t.setPosition(30, 320);
			win.draw(t);
		}
	}

	// Draw the current focused object's name
	SetGuiView();
	sf::Text t(m_focus->GetName(), m_gui.getGlobalFont(), 24);
	t.setPosition(20, 10);
	win.draw(t);
	SetStateView();

	CommonState::Draw(win);
}

void States::PlayerState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled)
{
	// Return if the GUI already handled it
	if (guiHandled)
		return;

	// Don't interact if focused object cannot be controlled
	if( !m_focus->IsLocatedOnAMap() || !m_focus->HasProperty( STR_PROP_OWNER ) ||
		m_focus->GetProperty( STR_PROP_OWNER ).Value() != m_player->GetName() )
		return;
	if( g_Game->GetWorld()->IsPaused() ) return;

	int tileX = (int)tilePos.x;
	int tileY = (int)tilePos.y;

	switch (button.button)
	{
	case sf::Mouse::Left: {
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


void States::PlayerState::MouseButtonReleased( sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, float time, bool guiHandled)
{
	// Return if the GUI already handled it
	if (guiHandled)
		return;

	if( g_Game->GetWorld()->IsPaused() ) return;

	// Handle drop-event of drag&drop action
	if( m_draggedContent && GetCurrentMap())
	{
		if( m_draggedContent->from == Interfaces::DragContent::PROPERTY_PANEL
			&& m_draggedContent->object )
		{
			if( m_draggedContent->object->HasProperty( STR_PROP_ITEM ) )
			{
				// Take away from the source object
				m_draggedContent->prop->RemoveObject(m_draggedContent->object->ID());
				// Insert into map
				int x = (int)floor(tilePos.x);
				int y = (int)floor(tilePos.y);
				GetCurrentMap()->Add( m_draggedContent->object->ID(), x, y, AutoDetectLayer(m_draggedContent->object) );
			}
		}
		delete m_draggedContent;
		m_draggedContent = nullptr;
	}
}


void States::PlayerState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled)
{
	CommonState::MouseWheelMoved(wheel, guiHandled);

	// Manually scroll panels (tgui::callback not provided for this action).
	// The scroll call has an effect only if the mouse is on the respective
	// element. So calling it for everybody is just fine.
	m_playerView->Scroll(wheel.delta);
	m_observerView->Scroll(wheel.delta);
}


void States::PlayerState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
	// Everything in that damned gui eats the focus - only edits are using it
	tgui::EditBox* edit = dynamic_cast<tgui::EditBox*>(GameState::GetFocusedElement().get());
	guiHandled &= edit != nullptr;

	// Handle copy & paste
	GameState::KeyPressed(key, guiHandled);

	// Let common state handle input
	CommonState::KeyPressed(key, guiHandled);

	if( key.code >= sf::Keyboard::Num1 && key.code <= sf::Keyboard::Num9 )
	{
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
			if( m_focus != nullptr )
				SetHotkeyToObject(key.code - sf::Keyboard::Num1, m_focus->ID());
		} else
			FocusObject( GetObjectFromHotkey(key.code - sf::Keyboard::Num1) );
	}

	switch(key.code) //pre gui switch to still get event with LControl
	{
	// Pre-GUI Tabbing to prevent loosing focus (focus tabbing of gui cannot be
	// deactivated.
	case sf::Keyboard::Tab: {
		// not allowed in combat
		if (m_combat)
			break;

		Core::Object* object;
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::LShift ) )
			object = g_Game->GetWorld()->GetNextObservableObject(m_focus->ID(), -1);
		else
			object = g_Game->GetWorld()->GetNextObservableObject(m_focus->ID(), 1);

		if(object != nullptr)
			FocusObject(object);

		return; } break;
	}

	// Don't react to any key if gui handled it
	if (guiHandled)
		return;

	switch(key.code)
	{
	case sf::Keyboard::Num0:
	case sf::Keyboard::Numpad0:
		// Refocus on player
		m_focus = m_player;
		break;
	case sf::Keyboard::LAlt:
		m_focus = m_player;
		break;
	case sf::Keyboard::C:
		g_Game->GetStateMachine()->PushGameState(new CharacterState(&m_playerObjectID));
		break;
	}
}



void States::PlayerState::OnBegin()
{
	tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );
	m_playerView->Init( 624.0f, 0.0f, 400.0f, localOut->getPosition().y, false, false,
		m_playerID, &m_draggedContent );
	m_playerView->Show( g_Game->GetWorld(), m_player );

	// Auto select the player (he is the actor)
	this->AddToSelection(m_player->ID());

	GetStateView() = g_Game->GetWindow().getView();
}


void States::PlayerState::Update( float _dt )
{
	CommonState::Update( _dt );

	if (!g_Game->GetWorld()->GetObject(m_playerObjectID))
	{
		m_player = nullptr;
		return;
	}

	// Go through player object and update all viewed properties
	m_playerView->Show( g_Game->GetWorld(), m_player );

	// Hide observer view if player is too far away
	if( m_observerView->isVisible() )
	{
		if( sfUtils::LengthSq( m_observerView->GetObjects()[0]->GetPosition() - m_player->GetPosition() ) > 3.0f )
			m_observerView->hide();
		else m_observerView->RefreshFilter();
	}
}


void States::PlayerState::Resize(const sf::Vector2f& _size)
{
	// Scale chat too
	CommonState::Resize( _size );

	tgui::ChatBox::Ptr localOut = m_gui.get( "Messages" );

	// Scale player view
	m_playerView->setSize( m_playerView->getSize().x, localOut->getPosition().y );
	m_playerView->setPosition( _size.x - m_playerView->getSize().x, 0.0f );

	if (InCombat())
	{
		m_observerView->setSize( 300.0f, _size.y - m_combatantPanel->getSize().y);
		m_combatantPanel->Resize(_size, m_observerView->getSize().x);
	}
	else
	{
		m_observerView->setSize( 300.0f, _size.y );
	}
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

void States::PlayerState::FocusObject(Core::Object* _object)
{
	if( !_object ) return;
	m_focus = _object;

	// Always have the current focused element in selection
	m_selection.Clear();
	m_selection.Add( _object->ID() );
}

void States::PlayerState::ExamineObject(Core::ObjectID _object)
{
	Core::Object* object = g_Game->GetWorld()->GetObject(_object);
	if( sfUtils::LengthSq( object->GetPosition() - m_player->GetPosition() ) > 3.0f )
	{
		g_Game->AppendToChatLog( Network::ChatMsg(STR_TO_FAR_AWAY, sf::Color::Red) );
	} else {
		m_observerView->Show( g_Game->GetWorld(), object );
		m_observerView->show();
	}
}


void States::PlayerState::CreateCombat( Core::ObjectID _object )
{
	// Find the object
	Core::Object* object = g_Game->GetWorld()->GetObject(_object);

	// Does this object belong to me?
	if ( this->OwnsObject(_object) )
	{
		// Maybe create a new Combat object
		if ( !m_combat )
			m_combat = new GameRules::Combat();

		// Show the combatant display
		m_combatantPanel->show();
		Resize(sf::Vector2f((float) g_Game->GetWindow().getSize().x,
			(float) g_Game->GetWindow().getSize().y));

		// Stop object
		if (object->HasProperty(STR_PROP_PATH))
		{
			Core::Property& path = object->GetProperty(STR_PROP_PATH);
			path.ClearObjects();
			path.SetValue(STR_FALSE);
		}

		// Prompt for initiative roll
		m_combat->PushInitiativePrompt(_object);
	}
}

bool States::PlayerState::OwnsObject( Core::ObjectID _object )
{
	// Find the object
	Core::Object* object = g_Game->GetWorld()->GetObject(_object);

	// True only if the object is found, has an owner and that owner is me
	return object != 0
		&& object->HasProperty(STR_PROP_OWNER)
		&& object->GetProperty(STR_PROP_OWNER).Value() == m_name;
}

	void States::PlayerState::CreateDiceRollState()
	{
		CommonState::CreateDiceRollState();
		m_diceRollState->AddButton("Würfeln", std::bind(&PlayerState::RollDice, this, std::placeholders::_1), sf::Keyboard::Return);
		m_diceRollState->AddButton("Abbrechen", [](const std::string&)->void{}, sf::Keyboard::Escape);
	}

	
	void States::PlayerState::RollDice(const std::string& _result)
	{
		Core::Object* obj = m_focus;
		if (!m_focus->HasProperty(STR_PROP_OWNER) || m_focus->GetProperty(STR_PROP_OWNER).Value() != m_name)
			obj = g_Game->GetWorld()->GetObject(m_playerObjectID);

		try {
			int x = Utils::EvaluateFormula(_result, g_Game->RANDOM, obj);
			// Output for all
			Network::ChatMsg("["+obj->GetName()+"] - Würfelwurf: "+_result+" = "+std::to_string(x), m_color).Send();
		} catch( Exception::InvalidFormula _e ) {
			g_Game->AppendToChatLog(Network::ChatMsg(_e.to_string(), sf::Color::Red));
		}
	}
