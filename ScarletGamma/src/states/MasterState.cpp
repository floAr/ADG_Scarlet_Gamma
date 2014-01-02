#include "states/MasterState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"
#include "states/StateMachine.hpp"
#include "Constants.hpp"
#include "utils/Falloff.hpp"
#include "network/Messenger.hpp"
#include <iostream>


void States::MasterState::Update(float dt)
{

	//Selection stuff
	
	if(m_selection){//user started a selection process
		if(m_selection->IsFinished()){//item was selected
			Core::ObjectID sel=m_selection->SelectedObject();
			delete(m_selection);
			m_selection=NULL;
			std::cout<<"user selected: "<<sel<<std::endl;
		}
	}



	m_zoom.Update(dt);
	if (m_zoom != 0)
		ZoomView(m_zoom);

	// Uses the test map 0 for testing purposes.
	g_Game->GetWorld()->GetMap(0)->Update(dt);

	// Handle network events
	Network::Messenger::Poll( false );
}

void States::MasterState::Draw(sf::RenderWindow& win)
{
	// Hard coded test selection:
	if( !m_selected ) {
		m_selected = g_Game->GetWorld()->GetObject(g_Game->GetWorld()->GetMap(0)->GetObjectsAt(0,2)[1]);
		m_player = m_selected;
	}

	// Draw some color to the background
	static sf::Color c(20, 26, 36);
	win.clear(c);

	// Render
	// Uses the test map 0 for testing purposes.
	Graphics::TileRenderer::Render(win, *g_Game->GetWorld()->GetMap(0));

	// If the selected object has a path draw it
	DrawPathOverlay(win);
}

void States::MasterState::MouseMoved(int deltaX, int deltaY)
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

void States::MasterState::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos)
{
	switch (button.button)
	{
	case sf::Mouse::Left: {
		//------------------------------------//
		// move player to tile position		  //
		//------------------------------------//
		auto& tiles = g_Game->GetWorld()->GetMap(0)->GetObjectsAt((int)tilePos.x,(int)tilePos.y);
		if( tiles.Size() > 0 )
		{
			// TODO: intelligent selcet?
			m_selected = g_Game->GetWorld()->GetObject(tiles[tiles.Size()-1]);
			// Delete current target(s) if not appending
			if( !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
			{
				m_player->GetProperty(Core::Object::PROP_TARGET).SetValue("");
				Core::Property& path = m_player->GetProperty(Core::Object::PROP_PATH);
				path.ClearObjects();
				path.SetValue("false");
			}
			if(m_selected->GetProperty(Core::Object::PROP_LAYER).Value()=="0"){
				// Append to target list
				m_player->AppendToPath( m_selected->ID() );
			}
		}
						  } break;
	case sf::Mouse::Middle:{
		m_zoom = 0;
		sf::RenderWindow& win = g_Game->GetWindow();
		sf::View newView = win.getView();
		newView.setSize((float)win.getSize().x, (float)win.getSize().y);
		win.setView(newView);
						   }
						   break;
	case sf::Mouse::Right:{
		if(m_selection)
			delete(m_selection);
		m_selection=new States::SelectionState();
		m_selection->AddTilePosition((int)tilePos.x,(int)tilePos.y);
		g_Game->GetStateMachine()->PushGameState(m_selection);
						  }

						  break;
	}
}

void States::MasterState::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel)
{
	m_zoom = (float)wheel.delta;
}

void States::MasterState::ZoomView(float delta)
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

void States::MasterState::DrawPathOverlay(sf::RenderWindow& win)
{
	assert( m_player );
	try {
		std::vector<sf::Vector2i> path;
		sf::Vector2i start(sfUtils::Round(m_player->GetPosition()));
		if( m_player->HasProperty("Path") )
		{
			// For each way point compute the shortest path
			auto& wayPoints = m_player->GetProperty("Path").GetObjects();
			for( int i=0; i<wayPoints.Size(); ++i )
			{
				sf::Vector2i goal = sfUtils::Round(g_Game->GetWorld()->GetObject(wayPoints[i])->GetPosition());
				auto part = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
				start = goal;
				path.insert( path.end(), part.begin(), part.end() );
			}
		} else if( m_player->HasProperty("Target") ) {
			// There are no paths but a short time target.
			sf::Vector2i goal = sfUtils::Round(sfUtils::to_vector(m_player->GetProperty("Target").Value()));
			path = g_Game->GetWorld()->GetMap(0)->FindPath(start, goal);
		}

		if( path.size() > 0 )
			Graphics::TileRenderer::RenderPath(win, path);
	} catch(...) {
		// In case of an invalid selection just draw no path
	}
}


void States::MasterState::OnBegin()
{
	// Init server
	Network::Messenger::Initialize(sf::IpAddress());

	// Load the test map
	Jo::Files::HDDFile file("saves/unittest.json");
	g_Game->GetWorld()->Load( file );

}

void States::MasterState::OnEnd()
{
	Network::Messenger::Close();
}
