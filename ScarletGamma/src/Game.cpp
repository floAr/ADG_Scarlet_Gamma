#include "Game.hpp"
#include "states/StateMachine.hpp"
#include "states/GameState.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/World.hpp"
#include "core/Map.hpp"

void Game::Init()
{
	m_stateMachine = new States::StateMachine();

	// Set up SFML window
	m_window.create(sf::VideoMode(800, 600), "Scarlet Gamma");
	m_window.setVerticalSyncEnabled(true);

	// Create an empty game world
	m_world = new Core::World();

	// Push states. Note that the last state is current!
	m_stateMachine->PushGameState(States::GST_MAIN_MENU);
	m_stateMachine->PushGameState(States::GST_INTRO);
}

void Game::Run()
{
	sf::Clock clock;
	while (m_window.isOpen() && m_stateMachine->HasStates())
	{
		// Get elapsed time
		sf::Time elapsed = clock.restart();

		// Forward windows events
		// This could also be handled by the GameState if required
		// If you need this, move it :)
		sf::Event event;
        while (m_window.pollEvent(event))
        {
            // close on [x]
            if (event.type == sf::Event::Closed)
			{
                m_window.close();
			}
        }

		// Update and draw the GameState
		m_stateMachine->Update(elapsed.asSeconds());
		m_stateMachine->Draw(m_window);

		// Swap buffers
		m_window.display();
	}
}

void Game::CleanUp()
{
	// Delete in reverse order of construction
	delete m_world;
	delete m_stateMachine;
}