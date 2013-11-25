#include "states/MainMenuState.hpp"
#include <iostream>

States::MainMenuState::MainMenuState() :
	GameState()
{
	// \todo I don't want my own resource management
	// \todo I want some way to report errors
	m_menuFont.loadFromFile("media/arial.ttf");
}

void States::MainMenuState::Update(float dt)
{
	// TODO get input
}

void States::MainMenuState::Draw(sf::RenderWindow& win)
{
	win.clear(sf::Color::Black);
	sf::Text t("This is some text!", m_menuFont, 24);
	t.setPosition(30, 30);
	win.draw(t);
}