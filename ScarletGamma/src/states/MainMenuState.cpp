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
	// TODO:: I don't want my own resource management
	// TODO:: I want some way to report errors
	m_menuFont.loadFromFile("media/arial.ttf");
}

void States::MainMenuState::Update(float dt)
{
}

void States::MainMenuState::Draw(sf::RenderWindow& win)
{
	win.clear(sf::Color::Black);
	sf::Text t("Press enter to see TileRenderer in action\nor escape to quit.", m_menuFont, 24);
	t.setPosition(30, 30);
	win.draw(t);
}

void States::MainMenuState::KeyPressed(sf::Event::KeyEvent key)
{
	switch (key.code)
	{
	// Playing state with enter
	case sf::Keyboard::Return:
		g_Game->GetStateMachine()->PushGameState(GST_PLAYER);
		break;
	// Quit with escape
	case sf::Keyboard::Escape:
		m_finished = true;
		break;
	}
}