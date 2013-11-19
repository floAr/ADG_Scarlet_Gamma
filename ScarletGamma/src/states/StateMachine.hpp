#pragma once

#include "Prerequisites.hpp"
#include <stack>

namespace States
{
	enum GameStateType { GST_INTRO, GST_MAIN_MENU };

	/// \brief Factory and manager for GameState implementations.
	/// This is used to keep track of the GameStates and intialize new ones.
	/// Their implementation remains hidden to the other classes.
	class StateMachine
	{
	public:
		/// \brief Creates the specified GameState and pushes it onto the stack.
		/// \detail To remove a GameState, the state has to mark itself as
		///		finished. The game then returns to the previous GameState or
		///		quits if it was the last one.
		void PushGameState(GameStateType gst);

		/// \brief Get the current game state, i.e. the top of the stack.
		///	\return Pointer to current GameState
		inline GameState* GetCurrentState()
		{
			return m_gameStates.top();
		}

		/// \brief Used for quitting the game when the last GameState has ended.
		/// \return true if at least one state is left, false otherwise
		inline bool HasStates()
		{
			return !m_gameStates.empty();
		}

	private:
		/// \brief Stack of GameStates. Current state is the top element.
		std::stack<GameState*> m_gameStates;
	};
}