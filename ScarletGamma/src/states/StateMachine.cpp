#include "StateMachine.hpp"
#include "IntroState.hpp"
#include "MainMenuState.hpp"

void States::StateMachine::PushGameState(States::GameStateType gst)
{
	switch(gst)
	{
	case GST_INTRO:
		m_gameStates.push(new States::IntroState());
		break;
	case GST_MAIN_MENU:
		m_gameStates.push(new States::MainMenuState());
		break;
	}
}