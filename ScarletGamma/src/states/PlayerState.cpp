#include "states/PlayerState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "Constants.hpp"
#include "utils/Falloff.hpp"
#include <iostream>

void States::PlayerState::Update(float dt)
{
	m_zoom.Update(dt);
	if (m_zoom != 0)
		ZoomView(m_zoom);

	// Uses the test map 0 for testing purposes.
	g_Game->GetWorld()->GetMap(0)->Update(dt);
}

void States::PlayerState::Draw(sf::RenderWindow& win)
{
	// Hard coded test selection:
	if( !m_selected )
		m_selected = g_Game->GetWorld()->GetObject(g_Game->GetWorld()->GetMap(0)->GetObjectsAt(0,2)[1]);

	// Draw some color to the background
	static sf::Color c(20, 26, 36);
	win.clear(c);

	// Render
	// Uses the test map 0 for testing purposes.
	Graphics::TileRenderer::Render(win, *g_Game->GetWorld()->GetMap(0));

	// If the selected object has a path draw it
	if( m_selected )
		DrawPathOverlay(win);
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

void States::PlayerState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos)
{
	switch (button.button)
	{
	case sf::Mouse::Left:
		//------------------------------------//
		// TODO: move player to tile position //
		//------------------------------------//
		break;
	case sf::Mouse::Middle:
		m_zoom = 0;
		sf::RenderWindow& win = g_Game->GetWindow();
		sf::View newView = win.getView();
		newView.setSize((float)win.getSize().x, (float)win.getSize().y);
		win.setView(newView);
		break;
	}
}

void States::PlayerState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel)
{
	m_zoom = (float)wheel.delta;
}

void States::PlayerState::ZoomView(float delta)
{
	// Get the render window
	sf::RenderWindow& win = g_Game->GetWindow();

	// Create a new view with its center shifted by the mouse position
	sf::Vector2f center = win.getView().getCenter();
	sf::View newView = win.getView();

	newView.zoom(exp(delta * 0.1f));

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

	// Apply view to the window
	win.setView(newView);
}

void States::PlayerState::DrawPathOverlay(sf::RenderWindow& win)
{
	try {
		std::vector<sf::Vector2i> path;
		sf::Vector2i start(sfUtils::Round(m_selected->GetPosition()));
		if( m_selected->HasProperty("Path") )
		{
			// For each way point compute the shortest path
			auto& wayPoints = m_selected->GetProperty("Path").Objects();
			for( int i=0; i<wayPoints.Size(); ++i )
			{
				sf::Vector2i goal = sfUtils::Round(g_Game->GetWorld()->GetObject(wayPoints[i])->GetPosition());
				auto part = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
				start = goal;
				path.insert( path.end(), part.begin(), part.end() );
			}
		} else if( m_selected->HasProperty("Target") ) {
			// There are no paths but a short time target.
			sf::Vector2i goal = sfUtils::Round(sfUtils::to_vector(m_selected->GetProperty("Target").Value()));
			path = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
		}

		if( path.size() > 0 )
			Graphics::TileRenderer::RenderPath(win, path);
	} catch(...) {
		// In case of an invalid selection just draw no path
	}
}
