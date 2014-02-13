#include "states/LaunchPlayerState.hpp"
#include "StateMachine.hpp"
#include "Game.hpp"
#include "SFML/Window.hpp"
#include <iostream>
#include "utils/Content.hpp"
#include "SFML/Network/IpAddress.hpp"
#include "PlayerState.hpp"
#include "network/Messenger.hpp"
#include "NewPlayerState.hpp"
#include "network/WorldMessages.hpp"
#include "core/World.hpp"

States::LaunchPlayerState::LaunchPlayerState() :
    GameState(),
	m_newPlayer(0xffffffff)
{
	m_menuFont = Content::Instance()->LoadFont("media/arial.ttf");

    //--------------------------------------
    // CREATE GUI
    m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont(m_menuFont);
	if( !m_gui.loadWidgetsFromFile( "media/LaunchPlayer.gui" ) )
		std::cout << "[LaunchPlayerState::LaunchPlayerState] Could not load GUI.\n";
    SetGui(&m_gui);
}




void States::LaunchPlayerState::Draw(sf::RenderWindow& win)
{
    // Set window color according to mouse position...
    win.clear(sf::Color::Black);

	GameState::Draw(win);
}

void States::LaunchPlayerState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
    switch (key.code)
    {
    // Quit with escape
    case sf::Keyboard::Escape:
        m_finished = true;
        break;
    }
}


void States::LaunchPlayerState::GuiCallback(tgui::Callback& callback)
{
	switch (callback.id)
	{
	case 1: m_finished = true; break;
	case 2: {
		// Player state
		tgui::EditBox::Ptr ipEdit = m_gui.get( "ServerIP" );
		tgui::EditBox::Ptr nameEdit = m_gui.get( "Name" );
		tgui::EditBox::Ptr r = m_gui.get("ColorR");
		tgui::EditBox::Ptr g = m_gui.get("ColorG");
		tgui::EditBox::Ptr b = m_gui.get("ColorB");
		sf::Color chatColor( atoi( r->getText().toAnsiString().c_str() ),
			atoi( g->getText().toAnsiString().c_str() ),
			atoi( b->getText().toAnsiString().c_str() ));

		// Show that we are working
		tgui::Label::Ptr status = m_gui.get("Status");
		status->setTextColor( sf::Color(200, 200, 200, 255) );
		status->setText("Connecting to server ...");
		Draw(*m_gui.getWindow());
		m_gui.draw();
		m_gui.getWindow()->display();

		// Connect to server
		try {
			sf::IpAddress adress(ipEdit->getText());
			Network::Messenger::Initialize(&adress);

			// The first packet contains the connection id
			auto mySocket = Network::Messenger::GetSocket(0);
			sf::Packet packet;	Core::PlayerID id;
			mySocket->receive( packet );
			packet >> id;

			// If necessary we must send the player object to the server now.
			if( m_newPlayer != 0xffffffff )
			{
				Network::MsgAddObject( m_newPlayer ).Send();
			} else {
				// Send a dummy packet.
				mySocket->send( sf::Packet() );
			}

			// Then the player mode can be created
			g_Game->GetStateMachine()->PushGameState(new PlayerState(nameEdit->getText(), chatColor, id));
		} catch(std::string _msg) {
			status->setTextColor( sf::Color(255, 55, 25, 255) );
			status->setText(_msg);
			m_gui.focusWidget( m_gui.get( "ServerIP" ) );
		}
		break; }
	case 3: {
		// Create a new player object first.
		g_Game->GetStateMachine()->PushGameState(new CharacterState(&m_newPlayer));
		break; }
	default:
		// No such GUI element!
		assert(false);
	}
}

void States::LaunchPlayerState::OnResume()
{
	auto name = g_Game->GetWorld()->GetObject( m_newPlayer )->GetName();
	tgui::EditBox::Ptr(m_gui.get("Name"))->setText( name );
}
