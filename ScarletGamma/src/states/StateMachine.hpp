#pragma once

#include "Prerequisites.hpp"
#include <SFML/Graphics.hpp>
#include <stack>

namespace States
{
	enum GameStateType { GST_INTRO, GST_MAIN_MENU, GST_PLAYER };

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
		/// \param [in] state  Identifies the state to be pushed
		void PushGameState(GameStateType state);

		/// \brief Used for quitting the game when the last GameState has ended.
		/// \return true if at least one state is left, false otherwise
		inline bool HasStates()
		{
			return m_gameState != 0;
		}

		/// \brief Updates the current GameState. May pop it if it's finished. 
		/// \param [in] dt  Delta time since last frame in seconds.
		void Update(float dt);

		/// \brief Draws the current GameState.
		void Draw(sf::RenderWindow& win);


		//----------------------------------------------------------------------
		// KEYBOARD EVENTS

		/// \brief Gets called by the InputHandler when any ASCII character was
		///		entered.
		/// \details You shouldn't call this function manually, except for
		///		cases where you really want to fake user input (e.g. buttons
		///		that insert special characters or something).
		/// \param [in] character  ASCII character that was entered.
		void TextEntered(char character);

		/// \brief Gets called by the InputHandler when any key is pressed.
		/// \param [in] key	SFML key event that contains all required information.
		void KeyPressed(sf::Event::KeyEvent& key);

		/// \brief Gets called by the InputHandler when any key is released.
		/// \param [in] key   SFML key event that contains all required information.
		/// \param [in] time  How long the key was pressed, in seconds.
		void KeyReleased(sf::Event::KeyEvent& key, float time);


		//----------------------------------------------------------------------
		// MOUSE EVENTS

		/// \brief Gets called when the mouse wheel is moved.
		/// param [in] wheel  SFML wheel event that contains all required information.
		void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel);

		/// \brief Gets called when a mouse button is pressed.
		/// \details Has the same internal logic as KeyPressed.
		/// param [in] wheel  SFML button event that contains all required information.
		void MouseButtonPressed(sf::Event::MouseButtonEvent& button);

		/// \brief Gets called when a mouse button is released.
		/// param [in] wheel  SFML button event that contains all required information.
		/// \param [in] time  How long the button was pressed, in seconds.
		void MouseButtonReleased(sf::Event::MouseButtonEvent& button, float time);

		/// \brief Gets called when the mouse is moved.
		/// param [in] wheel  SFML move event that contains all required information.
		void MouseMoved(sf::Event::MouseMoveEvent& move);

	private:
		/// \brief Pointer to the current GameState. The previous state is known
		///		by the current GameState.
		GameState* m_gameState;
	};
}