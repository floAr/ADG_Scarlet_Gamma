#include <cassert>

#include "Game.hpp"
#include "Constants.hpp"
#include "CommonState.hpp"
#include "network/Messenger.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Object.hpp"
#include "core/World.hpp"

namespace States {

CommonState::CommonState() :
		m_zoom(Utils::Falloff::FT_QUADRATIC, 0.75f, 0.05f),
		m_selected(nullptr)
{

}


void CommonState::OnEnd()
{
	Network::Messenger::Close();
}



void CommonState::Update( float dt )
{
	// Handle network events
	Network::Messenger::Poll( false );

	m_zoom.Update(dt);
	if( m_zoom != 0 )
		ZoomView(m_zoom);
}


void CommonState::MouseMoved(int deltaX, int deltaY)
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


void CommonState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel)
{
	m_zoom = (float)wheel.delta;
}



void CommonState::ZoomView(float delta)
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


void CommonState::DrawPathOverlay(sf::RenderWindow& win, Core::Object* _whosePath)
{
	assert( _whosePath );
	try {
		std::vector<sf::Vector2i> path;
		sf::Vector2i start(sfUtils::Round(_whosePath->GetPosition()));
		if( _whosePath->HasProperty(Core::Object::PROP_PATH) )
		{
			// For each way point compute the shortest path
			Core::Property& pathProperty = _whosePath->GetProperty(Core::Object::PROP_PATH);
			auto& wayPoints = pathProperty.GetObjects();
			for( int i=0; i<wayPoints.Size(); ++i )
			{
				sf::Vector2i goal = sfUtils::Round(g_Game->GetWorld()->GetObject(wayPoints[i])->GetPosition());
				auto part = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
				start = goal;
				path.insert( path.end(), part.begin(), part.end() );
			}
			// Loop?
			if( pathProperty.Value() == "true" )
			{
				sf::Vector2i goal = sfUtils::Round(g_Game->GetWorld()->GetObject(wayPoints[0])->GetPosition());
				auto part = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
				path.insert( path.end(), part.begin(), part.end() );
			}
		} else if( _whosePath->HasProperty(Core::Object::PROP_TARGET) ) {
			// There are no paths but a short time target.
			sf::Vector2i goal = sfUtils::Round(sfUtils::to_vector(_whosePath->GetProperty(Core::Object::PROP_TARGET).Value()));
			path = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
		}

		if( path.size() > 0 )
			Graphics::TileRenderer::RenderPath(win, path);
	} catch(...) {
		// In case of an invalid selection just draw no path
	}
}

} // namespace States