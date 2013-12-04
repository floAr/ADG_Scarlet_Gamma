#pragma once
#include <SFML/Window.hpp>
#include "Prerequisites.hpp"
#include <unordered_map>

namespace Events
{
	/// \brief Handles all input-type events that the SFML window generates.
	/// \details The events are pretty much forwarded to the StateMachines that
	///		is passed upon construction, which in turn forwards them to the
	///		current GameState.
	class InputHandler
	{
	public:
		/// \brief Create a new InputHandler. The Game has an EventHandler,
		///		and that should be the only guy creating an InputHandler.
		/// \param [in] stateMachine	Reference to the Game's StateMachine.
		InputHandler(States::StateMachine& stateMachine);

		/// \brief Updates internal timers.
		/// \param [in] dt    Delta time since last frame in seconds.
		void Update(float dt);


		//----------------------------------------------------------------------
		// KEYBOARD EVENTS

		/// \brief Gets called when a valid ASCII character is typed.
		/// \param [in] character    The typed character. May be something cool like à.
		void TextEntered(char character);

		/// \brief Gets called when a key is pressed.
		/// \details This starts a timer that is stopped when the key is released.
		///		The information is then used in the KeyReleased function to tell the
		///		StateMachine (i.e. the GameState) how long the key was pressed.
		/// \param [in] key    SFML key event that contains all required information.
		void KeyPressed(sf::Event::KeyEvent& key);

		/// \brief Gets called when a key is released.
		/// \param [in] key    SFML key event that contains all required information.
		void KeyReleased(sf::Event::KeyEvent& key);


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
		void MouseButtonReleased(sf::Event::MouseButtonEvent& button);

		/// \brief Gets called when the mouse is moved.
		/// param [in] wheel  SFML move event that contains all required information.
		void MouseMoved(sf::Event::MouseMoveEvent& move);

	private:
		/// \brief Counts the total time the InputManager was updated with. Used for
		///		determining how long a key / button was pressed.
		float m_totalTime;

		/// \brief For all pressed keys, it keeps the time when it was pressed.
		std::unordered_map<sf::Keyboard::Key, float> m_keyLastPressed;

		/// \brief For all pressed mouse buttons, it keeps the time when it was pressed.
		std::unordered_map<sf::Mouse::Button, float> m_mouseBtnLastPressed;

		/// \brief Reference to the StateMachine managed by the Game, used for
		///		getting the current GameState to inform it about input.
		States::StateMachine& m_stateMachine;

		unsigned int m_mousePos[2];
	};
}