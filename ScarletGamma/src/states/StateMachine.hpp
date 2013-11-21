#pragma once

#include "Prerequisites.hpp"
#include <SFML/Graphics.hpp>
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
		/// \brief Creates a new StateMachine. Initializes the current GameState to 0.
		StateMachine();

		/// \brief Creates the specified GameState and pushes it onto the stack.
		/// \detail To remove a GameState, the state has to mark itself as
		///		finished. The game then returns to the previous GameState or
		///		quits if it was the last one.
		/// \param [in] state	Identifies the state to be pushed
		void PushGameState(GameStateType state);

		/// \brief Used for quitting the game when the last GameState has ended.
		/// \return true if at least one state is left, false otherwise
		inline bool HasStates()
		{
			return m_gameState != 0;
		}

		/// \brief Updates the current GameState. May pop it if it's finished. 
		/// \param [in] dt	Delta time since last frame in seconds.
		void Update(float dt);

		/// \brief Draws the current GameState.
		void Draw(sf::RenderWindow& win);
	private:
		/// \brief Stack of GameStates. Current state is the top element.
		GameState* m_gameState;
	};
}