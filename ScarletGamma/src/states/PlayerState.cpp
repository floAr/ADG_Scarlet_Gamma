#include "states/PlayerState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.h"
#include <iostream>

void States::PlayerState::Update(float dt)
{
}

void States::PlayerState::Draw(sf::RenderWindow& win)
{
	// Draw some color to the background
	static sf::Color c(20, 26, 36);
	win.clear(c);

	// Render
	// Uses the test map 0 for testing purposes.
	Graphics::TileRenderer::Render(win, *g_Game->GetWorld()->GetMap(0));
}

void States::PlayerState::MouseMoved(int deltaX, int deltaY)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// Get the render window
		sf::RenderWindow& win = g_Game->GetWindow();

		// Create a new view with its center shifted by the mouse position
		sf::Vector2f center = win.getView().getCenter();
		sf::View newView = win.getView();
		sf::Vector2f scale(newView.getSize().x / win.getSize().x,
			               newView.getSize().y / win.getSize().y);
		newView.setCenter(center.x - (deltaX * scale.x),
			              center.y - (deltaY * scale.y));

		// Apply view to the window
		win.setView(newView);
	}
}

void States::PlayerState::MouseButtonPressed(sf::Event::MouseButtonEvent& button)
{
	switch (button.button)
	{
	case sf::Mouse::Middle:
		sf::RenderWindow& win = g_Game->GetWindow();
		sf::View newView = win.getView();
		newView.setSize(win.getSize().x, win.getSize().y);
		win.setView(newView);
		break;
	}
}

void States::PlayerState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel)
{
	// Get the render window
	sf::RenderWindow& win = g_Game->GetWindow();

	// Create a new view with its center shifted by the mouse position
	sf::Vector2f center = win.getView().getCenter();
	sf::View newView = win.getView();

	newView.zoom(exp(wheel.delta * 0.25f));

	std::cout << wheel.delta << " " << newView.getSize().x;

	// Clamp scale so that one tile doesn't get smaller than 1 pixel
	// This is required to avoid overdraw, which blocks GPU threads and
	// leads to all pixels be drawn serially. Welcome, 1 fps!
	static const float clampFactor = 0.25f; 
	if (newView.getSize().x / win.getSize().x > clampFactor * TILESIZE ||
		newView.getSize().y / win.getSize().y > clampFactor * TILESIZE)
	{
		newView.setSize(win.getSize().x * TILESIZE * clampFactor,
			            win.getSize().y * TILESIZE * clampFactor);
	}

	if (newView.getSize().x / win.getSize().x < clampFactor || 
		newView.getSize().y / win.getSize().y < clampFactor)
	{
		newView.setSize(win.getSize().x * clampFactor, win.getSize().y * clampFactor);
	}

	std::cout << " " << newView.getSize().x / win.getSize().y << std::endl;


	// Apply view to the window
	win.setView(newView);
}
