#include "StateMachine.hpp"
#include "IntroState.hpp"
#include "PlayerState.hpp"
#include "MainMenuState.hpp"

States::StateMachine::StateMachine() :
	m_gameState(0)
{
}

void States::StateMachine::PushGameState(States::GameStateType state)
{
	// Create initialized pointer
	GameState* newState = 0;

	// Fill the pointer, if the GameStateType is known
	switch(state)
	{
	case GST_INTRO:
		newState = new States::IntroState(std::string("media/acagamics.png"), 2);
		break;
	case GST_MAIN_MENU:
		newState = new States::MainMenuState();
		break;
	case GST_PLAYER:
		newState = new States::PlayerState();
		break;
	}

	// If we have a new state, "push" it
	if (newState != 0)
	{
		newState->SetPreviousState(m_gameState);
		m_gameState = newState;
	}
}

void States::StateMachine::Update(float dt)
{
	// Replace game state with predecessor - may be 0
	if (m_gameState && m_gameState->IsFinished())
		m_gameState = m_gameState->GetPreviousState();

	// Update, if there is a GameState left
	if (m_gameState)
		m_gameState->Update(dt);
}

void States::StateMachine::Draw(sf::RenderWindow& win)
{
	// Draw GameState to window, if both exists
	if (win.isOpen() && m_gameState)
		m_gameState->Draw(win);
}

void States::StateMachine::TextEntered(char character)
{
	if (m_gameState)
		m_gameState->TextEntered(character);
}

void States::StateMachine::KeyPressed(sf::Event::KeyEvent key)
{
	if (m_gameState)
		m_gameState->KeyPressed(key);
}

void States::StateMachine::KeyReleased(sf::Event::KeyEvent key, float time)
{
	if (m_gameState)
		m_gameState->KeyReleased(key, time);
}