#include "states/MainMenuState.hpp"
#include "graphics/TileRenderer.hpp"
#include "graphics/EditList.hpp"
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

    //--------------------------------------
    // CREATE GUI
    // First, create a tgui::Gui object and load a font
    m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont(m_menuFont);

    // Now create stuff or load using loadWidgetsFromFile()
	m_gui.loadWidgetsFromFile( "media/MainMenu.gui" );

	static Graphics::EditList test(m_gui, 400.0f, 50.0f, 240.0f, 210.f, true, true, true, false, "Attribute");
	for(int i=0; i<13; ++i)
		test.Add("keks", std::to_string(i));
	test.Add("Spaceship", "XWing");
	test.AddNode( "Components" );

    // Finally, use SetGui() to activate the GUI (rendering, events, callbacks)
    SetGui(&m_gui);
}



void States::MainMenuState::Update(float dt)
{

}

void States::MainMenuState::Draw(sf::RenderWindow& win)
{
    // Set window color according to mouse position...
	static sf::Color mightySlate(85, 98, 112);
    win.clear(mightySlate);
    
    sf::Text t("Press 'm' to open MasterState\n"
               "or escape to quit.", m_menuFont, 24);
    t.setPosition(30, 30);
    win.draw(t);
}

void States::MainMenuState::KeyPressed(sf::Event::KeyEvent& key)
{
    switch (key.code)
    {
    // Master state with m
    case sf::Keyboard::M:
        g_Game->GetStateMachine()->PushGameState(new States::MasterState("saves/unittest.json"));
        break;
    // Quit with escape
    case sf::Keyboard::Escape:
        m_finished = true;
        break;
    }
}

void States::MainMenuState::MouseMoved(int deltaX, int deltaY)
{
    m_mousePos[0] = sf::Mouse::getPosition().x;
    m_mousePos[1] = sf::Mouse::getPosition().y;
}

void States::MainMenuState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel)
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
	default:
		// No such GUI element!
		assert(false);
	}
}