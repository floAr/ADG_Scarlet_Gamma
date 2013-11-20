#include "StateMachine.hpp"
#include "IntroState.hpp"
#include "MainMenuState.hpp"

void States::StateMachine::PushGameState(States::GameStateType state)
{
	switch(state)
	{
	case GST_INTRO:
		m_gameStates.push(new States::IntroState(std::string("media/acagamics.png"), 2));
		break;
	case GST_MAIN_MENU:
		m_gameStates.push(new States::MainMenuState());
		break;
	}
}

void States::StateMachine::Update(float dt)
{
	// Pop GameState?
	if (!m_gameStates.empty() && m_gameStates.top()->IsFinished())
	{
		m_gameStates.pop();
	}

	// Update remaining GameState, if there is one
	if (!m_gameStates.empty())
	{
		m_gameStates.top()->Update(dt);
	}
}

void States::StateMachine::Draw(sf::RenderWindow& win)
{
	// Draw GameState to window, if both exists
	if (win.isOpen() && !m_gameStates.empty())
	{
		m_gameStates.top()->Draw(win);

		// Swap buffers
		win.display();
	}
}