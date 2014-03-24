#include "states/MainMenuState.hpp"
#include "graphics/TileRenderer.hpp"
#include "StateMachine.hpp"
#include "core/Map.hpp"
#include "Game.hpp"
#include "SFML/Window.hpp"
#include <iostream>
#include "utils/Content.hpp"
#include "MasterState.hpp"

States::MainMenuState::MainMenuState() :
    GameState()
{
    for (int i = 0; i < sizeof(m_mousePos) / sizeof(m_mousePos[0]); i++)
        m_mousePos[i] = 0;

    // TODO:: I want some way to report errors
	m_menuFont=Content::Instance()->LoadFont("media/arial.ttf");

	m_background = sf::Sprite(Content::Instance()->LoadTexture("media/main_bg.png"));
    //--------------------------------------
    // CREATE GUI
    // First, create a tgui::Gui object and load a font
    m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont(m_menuFont);

    // Now create stuff or load using loadWidgetsFromFile()
	m_gui.loadWidgetsFromFile( "media/MainMenu.gui" );

    // Finally, use SetGui() to activate the GUI (rendering, events, callbacks)
    SetGui(&m_gui);
}



void States::MainMenuState::Update(float dt)
{

}

void States::MainMenuState::Draw(sf::RenderWindow& win)
{
   // win.clear(MIGHTY_SLATE);
	SetGuiView();
	float sx = float(win.getSize().x) / m_background.getTexture()->getSize().x;
	float sy = float(win.getSize().y) / m_background.getTexture()->getSize().y;
	m_background.setScale( std::max(1.0f, sx), std::max(1.0f, sy) );
	win.draw(m_background);

   /* sf::Text t("Press 'm' to open MasterState\n"
               "or escape to quit.", m_menuFont, 24);
    t.setPosition(30, 30);
    win.draw(t);*/

	GameState::Draw(win);
}

void States::MainMenuState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
	// Return if the GUI already handled it
	if (guiHandled)
		return;

    switch (key.code)
    {
#ifdef _DEBUG
    // Master state with m
    case sf::Keyboard::M:{
       MasterState* ms=static_cast<MasterState*>( g_Game->GetStateMachine()->PushGameState(new States::MasterState("saves/unittest.json")));
	   ms->CreateDiceRollState();}
        break;
#endif
    // Quit with escape
    case sf::Keyboard::Escape:
        m_finished = true;
        break;
    }
}

void States::MainMenuState::MouseMoved(int deltaX, int deltaY, bool guiHandled)
{
    m_mousePos[0] = sf::Mouse::getPosition().x;
    m_mousePos[1] = sf::Mouse::getPosition().y;
}

void States::MainMenuState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled)
{
    m_mousePos[2] += wheel.delta * 10;
}

void States::MainMenuState::GuiCallback(tgui::Callback& callback)
{
	switch (callback.id)
	{
	case 1: m_finished = true; break;
	case 2:
		// Master state initialization
		g_Game->GetStateMachine()->PushGameState(GST_LAUNCH_MASTER);
		break;
	case 3:
		// Player state initialization
		g_Game->GetStateMachine()->PushGameState(GST_LAUNCH_PLAYER);
		break;
	case 4:
		// Show credits
		g_Game->GetStateMachine()->PushGameState(GST_CREDITS);
		break;
	default:
		// No such GUI element!
		assert(false);
	}
}
