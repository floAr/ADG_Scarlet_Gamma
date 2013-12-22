#include "states/MainMenuState.hpp"
#include "graphics/TileRenderer.hpp"
#include "StateMachine.hpp"
#include "core/Map.hpp"
#include "Game.hpp"
#include "SFML/Window.hpp"
#include <iostream>

States::MainMenuState::MainMenuState() :
    GameState()
{
    for (int i = 0; i < sizeof(m_mousePos) / sizeof(m_mousePos[0]); i++)
        m_mousePos[i] = 0;

    // TODO:: I don't want my own resource management
    // TODO:: I want some way to report errors
    m_menuFont.loadFromFile("media/arial.ttf");

    //--------------------------------------
    // EXAMPLE GUI USAGE
    // First, create a tgui::Gui object and load a font
    m_gui.setWindow(g_Game->GetWindow());
    m_gui.setGlobalFont(m_menuFont);
    // Now create stuff or load using loadWidgetsFromFile()
    tgui::Button::Ptr button(m_gui);
    button->load("lib/TGUI-0.6-RC/widgets/Black.conf"); // TODO: this causes an exception later when main() finishes. I don't get it all :)
    button->setPosition(30, 200);
    button->setText("Quit");
    button->setCallbackId(1);
    button->bindCallback(tgui::Button::LeftMouseClicked);
    button->setSize(300, 40);
    // Finally, use SetGui() to activate the GUI (rendering, events, callbacks)
    SetGui(&m_gui);
}



void States::MainMenuState::Update(float dt)
{

}

void States::MainMenuState::Draw(sf::RenderWindow& win)
{
    // Set window color according to mouse position...
    win.clear(sf::Color((unsigned char)(255 * (float)m_mousePos[0] / (float)win.getSize().x),
        (unsigned char)(255 * (float)m_mousePos[1] / (float)win.getSize().y),
        (unsigned char)(m_mousePos[2])));
    
    sf::Text t("Press enter to open PlayerState\n"
               "or 'm' to open MasterState\n"
               "or escape to quit.\n\n"
               "Move mouse or scroll to change background color.", m_menuFont, 24);
    t.setPosition(30, 30);
    win.draw(t);
}

void States::MainMenuState::KeyPressed(sf::Event::KeyEvent& key)
{
    switch (key.code)
    {
    // Playing state with enter
    case sf::Keyboard::Return:
        g_Game->GetStateMachine()->PushGameState(GST_PLAYER);
        break;
    // Master state with m
    case sf::Keyboard::M:
        g_Game->GetStateMachine()->PushGameState(GST_MASTER);
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
    if (callback.id == 1)
        g_Game->GetWindow().close();
}