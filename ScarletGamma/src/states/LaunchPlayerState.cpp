#include "states/LaunchPlayerState.hpp"
#include "StateMachine.hpp"
#include "Game.hpp"
#include "SFML/Window.hpp"
#include <iostream>
#include "utils/Content.hpp"
#include "SFML/Network/IpAddress.hpp"
#include "ChoosePlayerState.hpp"
#include "network/Messenger.hpp"
#include "network/WorldMessages.hpp"
#include "core/World.hpp"

States::LaunchPlayerState::LaunchPlayerState() :
    GameState()
{
	m_menuFont = Content::Instance()->LoadFont("media/arial.ttf");

    //--------------------------------------
    // CREATE GUI
    m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont(m_menuFont);
	if( !m_gui.loadWidgetsFromFile( "media/LaunchPlayer.gui" ) )
		std::cout << "[LaunchPlayerState::LaunchPlayerState] Could not load GUI.\n";
    SetGui(&m_gui);
	tgui::Label::Ptr(m_gui.get("Status"))->setText("");

	m_background = sf::Sprite(Content::Instance()->LoadTexture("media/main_bg.png"));
	m_logo = sf::Sprite(Content::Instance()->LoadTexture("media/logo.png"));
	m_logo.setPosition(10, 25);
}




void States::LaunchPlayerState::Draw(sf::RenderWindow& win)
{
	SetGuiView();
	float sx = float(win.getSize().x) / m_background.getTexture()->getSize().x;
	float sy = float(win.getSize().y) / m_background.getTexture()->getSize().y;
	m_background.setScale( std::max(1.0f, sx), std::max(1.0f, sy) );
	win.draw(m_background);
	win.draw(m_logo);

	GameState::Draw(win);
}

void States::LaunchPlayerState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
	// Handle copy & paste
	GameState::KeyPressed(key, guiHandled);

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

		// Show that we are working
		tgui::Label::Ptr status = m_gui.get("Status");
		status->setTextColor( sf::Color(200, 200, 200, 255) );
		status->setText("Verbinde zu Server ...");
		Draw(*m_gui.getWindow());
		m_gui.draw();
		m_gui.getWindow()->display();

		// Connect to server
		try {
			// Options where ok -> save
			m_gui.saveWidgetsToFile( "media/LaunchPlayer.gui" );

			sf::IpAddress adress(ipEdit->getText());
			Network::Messenger::Initialize(&adress);

			// The first packet contains the connection id
			auto mySocket = Network::Messenger::GetSocket(0);
			sf::Packet packet;	Core::PlayerID id;
			mySocket->receive( packet );
			packet >> id;

			// Then the player mode can be created
			g_Game->GetStateMachine()->PushGameState(new ChoosePlayerState(id));

		} catch(std::string _msg) {
			status->setTextColor( sf::Color(255, 55, 25, 255) );
			status->setText(_msg);
			m_gui.focusWidget( m_gui.get( "ServerIP" ) );
		}
		break; }
	default:
		// No such GUI element!
		assert(false);
	}
}
