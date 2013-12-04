#include "states/PlayerState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"

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

void States::PlayerState::MouseMoved(sf::Event::MouseMoveEvent& move)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// Get the render window
		sf::RenderWindow& win = g_Game->GetWindow();

		// Create a new view with its center shifted by the mouse position
		sf::Vector2f center = win.getView().getCenter();
		sf::View newView = win.getView();
		newView.setCenter(center.x - (move.x - m_mousePos[0]),
						  center.y - (move.y - m_mousePos[1]));

		// Apply view to the window
		win.setView(newView);
	}

	m_mousePos[0] = move.x;
	m_mousePos[1] = move.y;
}