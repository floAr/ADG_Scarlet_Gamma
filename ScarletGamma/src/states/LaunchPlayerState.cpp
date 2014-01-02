#include "states/LaunchPlayerState.hpp"
#include "StateMachine.hpp"
#include "Game.hpp"
#include "SFML/Window.hpp"
#include <iostream>
#include "utils/Content.hpp"
#include "SFML/Network/IpAddress.hpp"
#include "PlayerState.hpp"
#include "network/Messenger.hpp"

States::LaunchPlayerState::LaunchPlayerState() :
    GameState()
{
	m_menuFont=Content::Instance()->LoadFont("media/arial.ttf");

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
}

void States::LaunchPlayerState::KeyPressed(sf::Event::KeyEvent& key)
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

		tgui::Label::Ptr status = m_gui.get("Status");
		status->setTextColor( sf::Color(200, 200, 200, 255) );
		status->setText("Connecting to server ...");
		Draw(*m_gui.getWindow());
		m_gui.draw();
		m_gui.getWindow()->display();

		// Connect to server: local host for tests
		try {
			sf::IpAddress adress(ipEdit->getText());
			Network::Messenger::Initialize(&adress);

			// Then the player mode can be created
			g_Game->GetStateMachine()->PushGameState(GST_PLAYER);
		} catch(std::string _msg) {
			status->setTextColor( sf::Color(255, 55, 25, 255) );
			status->setText(_msg);
			m_gui.focusWidget( m_gui.get( "ServerIP" ) );
		}
		} break;
	default:
		// No such GUI element!
		assert(false);
	}
}