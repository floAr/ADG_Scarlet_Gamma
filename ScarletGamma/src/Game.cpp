#include "Game.hpp"
#include "events/EventHandler.hpp"
#include "states/StateMachine.hpp"
#include "states/GameState.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/World.hpp"
#include "core/Map.hpp"
#include <math.h>
#include "sfutils/View.hpp"

void Game::Init()
{
	// Create a state machine
	m_stateMachine = new States::StateMachine();

	// Set up SFML window
	m_window.create(sf::VideoMode(800, 600), "Scarlet Gamma");
	m_window.setVerticalSyncEnabled(true);
	m_window.setKeyRepeatEnabled(true); // PLEASE LEAVE THIS ON, because the GUI will suck otherwise.
										// If this has a bad effect on gameplay, we'll need to find a workaround.
										//   ~Daerst

	// Create an event handler using the window
	m_eventHandler = new Events::EventHandler(m_window, *m_stateMachine);

	// Create an empty game world
	m_world = new Core::World();

	// Push states. Note that the last state is current!
	m_stateMachine->PushGameState(States::GST_MAIN_MENU);
	m_stateMachine->PushGameState(States::GST_INTRO);

#ifdef _FPS
	m_dFps = 60;
	m_dFpsTime = 0;
	m_dFpsCounter = 0;
	m_dFpsFont.loadFromFile("media/arial.ttf");
#endif
}

void Game::Run()
{
	sf::Clock clock;
	while (m_window.isOpen() && m_stateMachine->HasStates())
	{
		// Get elapsed time and add it to total running time
		float time = clock.restart().asSeconds();

		// Let the EventHandler handle all window events
		m_eventHandler->Update(time);

		// Update and draw the GameState
		m_stateMachine->Update(time);
		m_stateMachine->Draw(m_window);

#ifdef _FPS
		m_dFpsTime += time;
		m_dFpsCounter++;
		if (m_dFpsTime >= 1.f)
		{
			m_dFps = m_dFpsCounter;
			m_dFpsTime = 0;
			m_dFpsCounter = 0;
		}
		sf::View backup = sfUtils::View::SetDefault(&m_window);
		sf::Text t(std::to_string(m_dFps), m_dFpsFont, 12);
		if (m_dFps < 30)
			t.setColor(sf::Color::Red);
		t.setPosition(5, 5);
		m_window.draw(t);
		m_window.setView(backup);
#endif

		// Swap buffers
		m_window.display();
	}
}

void Game::CleanUp()
{
	// Delete in reverse order of construction
	delete m_world;
	delete m_eventHandler;
	delete m_stateMachine;
}