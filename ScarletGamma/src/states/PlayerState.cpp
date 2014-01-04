#include "states/PlayerState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "utils/Falloff.hpp"
#include "network/Messenger.hpp"
#include <iostream>


void States::PlayerState::Draw(sf::RenderWindow& win)
{
	// Draw some color to the background
	static sf::Color c(20, 26, 36);
	win.clear(c);

	// Render
	// Uses the test map 0 for testing purposes.
	Graphics::TileRenderer::Render(win, *g_Game->GetWorld()->GetMap(0));

	// If the selected object has a path draw it
	//DrawPathOverlay(win);
}

void States::PlayerState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos)
{
	switch (button.button)
	{
	case sf::Mouse::Middle:
		m_zoom = 0;
		sf::RenderWindow& win = g_Game->GetWindow();
		sf::View newView = win.getView();
		newView.setSize((float)win.getSize().x, (float)win.getSize().y);
		win.setView(newView);
		break;
	}
}



void States::PlayerState::OnBegin()
{
	// After connecting the client is receiving the world
	Network::Messenger::Poll( true );
}
