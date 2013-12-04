#include "Game.hpp"
#include "events/EventHandler.hpp"
#include "states/StateMachine.hpp"
#include "states/GameState.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/World.hpp"
#include "core/Map.hpp"

void Game::Init()
{
	// Create a state machine
	m_stateMachine = new States::StateMachine();

	// Set up SFML window
	m_window.create(sf::VideoMode(800, 600), "Scarlet Gamma");
	m_window.setVerticalSyncEnabled(true);
	m_window.setKeyRepeatEnabled(false);

	// Create an event handler using the window
	m_eventHandler = new Events::EventHandler(m_window, *m_stateMachine);

	// Create an empty game world
	m_world = new Core::World();
	// Load the test map
	m_world->Load("saves/unittest.json");

	// Push states. Note that the last state is current!
	m_stateMachine->PushGameState(States::GST_MAIN_MENU);
	m_stateMachine->PushGameState(States::GST_INTRO);
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