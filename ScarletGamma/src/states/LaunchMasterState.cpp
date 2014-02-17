#include "states/LaunchMasterState.hpp"
#include "StateMachine.hpp"
#include "Game.hpp"
#include "SFML/Window.hpp"
#include <iostream>
#include "utils/Content.hpp"
#include "SFML/Network/IpAddress.hpp"
#include "MasterState.hpp"

States::LaunchMasterState::LaunchMasterState() :
    GameState()
{
	m_menuFont=Content::Instance()->LoadFont("media/arial.ttf");

    //--------------------------------------
    // CREATE GUI
    m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont(m_menuFont);
	if( !m_gui.loadWidgetsFromFile( "media/LaunchMaster.gui" ) )
		std::cout << "[LaunchMasterState::LaunchMasterState] Could not load GUI.\n";
	tgui::EditBox::Ptr ipEdit = m_gui.get( "LanIP" );
	ipEdit->setText(sf::IpAddress::getLocalAddress().toString());
	ipEdit = m_gui.get( "PubIP" );
	ipEdit->setText(sf::IpAddress::getPublicAddress().toString());
    SetGui(&m_gui);
}




void States::LaunchMasterState::Draw(sf::RenderWindow& win)
{
    // Set window color according to mouse position...
    win.clear(sf::Color::Black);

	GameState::Draw(win);
}

void States::LaunchMasterState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
    switch (key.code)
    {
    // Quit with escape
    case sf::Keyboard::Escape:
        m_finished = true;
        break;
    }
}


void States::LaunchMasterState::GuiCallback(tgui::Callback& callback)
{
	switch (callback.id)
	{
	case 1: m_finished = true; break;
	case 2: {
		// Master state
		tgui::EditBox::Ptr nameEdit = m_gui.get( "FileName" );
		std::string fileName = nameEdit->getText();
		if( Jo::Files::Utils::Exists( fileName ) )
		{
			g_Game->GetStateMachine()->PushGameState(new States::MasterState(fileName));
			// Options are ok -> save
			tgui::EditBox::Ptr(m_gui.get("LanIP"))->setText("");	// Do not save personal information
			tgui::EditBox::Ptr(m_gui.get("PubIP"))->setText("");
			m_gui.saveWidgetsToFile( "media/LaunchMaster.gui" );
		} else {
			tgui::Label::Ptr(m_gui.get("Error"))->setText("File not found!");
			m_gui.focusWidget( m_gui.get( "FileName" ) );
		}
		} break;
	case 3: {
		// Create a new file for the map (if not existing)
		tgui::EditBox::Ptr nameEdit = m_gui.get( "FileName" );
		std::string fileName = nameEdit->getText();
		std::string dir, file;
		Jo::Files::Utils::SplitName(fileName, dir, file);
		Jo::Files::Utils::MakeDir(dir);
		fclose( fopen( fileName.c_str(), "wb" ) );
		break; }
	default:
		// No such GUI element!
		assert(false);
	}
}