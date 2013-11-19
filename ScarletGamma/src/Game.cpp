#include "Game.hpp"
#include "states/StateMachine.hpp"
#include "states/GameState.hpp"
#include "graphics/GraphicsManager.hpp"
#include <SFML/System.hpp>

void Game::Init()
{
	m_stateMachine = new States::StateMachine();
	m_graphicsManager = new Graphics::GraphicsManager();

	// Switch to intro state
	m_stateMachine->PushGameState(States::GST_INTRO);
}

void Game::Run()
{
	sf::Clock clock;
	while (m_stateMachine->HasStates())
	{
		// Get elapsed time
		sf::Time elapsed = clock.restart();
		
		// Get current state from the StateMachine
		States::GameState* state = m_stateMachine->GetCurrentState();

		// Update and draw the GameState
		state->Update(elapsed.asSeconds());
		state->Draw();
	}
}

void Game::CleanUp()
{
	// Delete in reverse order of construction
	delete m_graphicsManager;
	delete m_stateMachine;
}