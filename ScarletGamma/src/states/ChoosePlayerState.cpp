#include "states/ChoosePlayerState.hpp"
#include "StateMachine.hpp"
#include "Game.hpp"
#include "utils/Content.hpp"
#include "core/World.hpp"
#include "network/Messenger.hpp"
#include "PlayerState.hpp"
#include "NewPlayerState.hpp"


namespace States {

ChoosePlayerState::ChoosePlayerState(Core::PlayerID _id) :
    GameState(),
	m_playerID(_id),
	m_newPlayer(0xffffffff)
{
	// The client is receiving the world after connecting
	Network::Messenger::Poll( true );

    //--------------------------------------
    // CREATE GUI
    m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont(Content::Instance()->LoadFont("media/arial.ttf"));
    SetGui(&m_gui);

	tgui::Button::Ptr createNew(m_gui);
	createNew->load("media/black.conf");
	createNew->setSize( 300.0f, 40.0f );
	createNew->setPosition( g_Game->GetWindow().getSize().x * 0.5f - 150.0f, 80.0f );
	createNew->setText( "Neuer Spieler" );

	auto players = g_Game->GetWorld()->GetAllPlayers();
	for( size_t i = 0; i < players.size(); ++i )
	{
		tgui::Button::Ptr button = createNew.clone();
		m_gui.add( button );
		button->setPosition( button->getPosition().x, 150.0f + i * 50.0f );
		button->setText( g_Game->GetWorld()->GetObject(players[i])->GetName() );
		button->setCallbackId( players[i] );
		button->bindCallbackEx( &ChoosePlayerState::ChoosePlayer, this, tgui::Button::LeftMouseClicked );
	}

	createNew->bindCallback( &ChoosePlayerState::CreatePlayer, this, tgui::Button::LeftMouseClicked );
}



void ChoosePlayerState::Draw(sf::RenderWindow& win)
{
    // Set window color according to mouse position...
    win.clear(sf::Color::Black);

	GameState::Draw(win);
}


void ChoosePlayerState::ChoosePlayer(const tgui::Callback& _callback)
{
	// Initialize player object
	Core::Object* player = g_Game->GetWorld()->GetObject( _callback.id );
	player->GetProperty(STR_PROP_PLAYER).SetValue( std::to_string(m_playerID) );

	// The player mode can be created
	g_Game->GetStateMachine()->PushGameState(new PlayerState(_callback.id));
	m_finished = true;
}


void ChoosePlayerState::CreatePlayer()
{
	// Create a new player object first.
	g_Game->GetStateMachine()->PushGameState(new CharacterState(&m_newPlayer));
}

void ChoosePlayerState::OnResume()
{
	// Might be called if stack is cleaned up at the end
	if( !m_finished && m_newPlayer!=0xffffffff )
	{
		tgui::Callback callback;
		callback.id = m_newPlayer;
		ChoosePlayer( callback );
	}
}


} // namespace States