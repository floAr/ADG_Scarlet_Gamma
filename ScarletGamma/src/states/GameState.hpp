#pragma once

#include <SFML/Graphics.hpp>

namespace States
{
	/// \brief Abstract base class for all states the game can be in.
	/// \details The StateMachine class is a factory and initializes and keeps
	///		track of the various subclasses used throughout the game.
	class GameState
	{
	public:
		/// \brief Empty base constructor that makes sure that the GameState
		///		isn't finished instantly.
		GameState() : m_finished(false), m_previousState(0) {}

		/// \brief Sets the previous GameState, i.e. the GameState that the
		///		StateMachine will return to when this one is finished.
		/// \detail If this is set to 0, the StateMachine will assume that
		///		no states are left and the game will quit. You shouldn't have
		///		to use this function, the StateMachine will set the previous
		///		state automatically when you push a new one.
		inline void SetPreviousState(GameState* state)
		{
			m_previousState = state;
		}

		/// \brief Returns a pointer to the previous GameState, i.e. the state
		///		that was on the top of the stack before this one was pushed.
		inline GameState* GetPreviousState()
		{
			return m_previousState;
		}

		/// \brief Pure virtual function that is called when the GameState is
		///		first entered and pushed.
		virtual void OnBegin() = 0;

		/// \brief Pure virtual function that is called when the GameState is
		///		finally pop'd from the stack.
		virtual void OnEnd() = 0;

		/// \brief Pure virtual function that is called when the GameState is
		///		paused due to another GameState being pushed on top of it.
		virtual void OnPause() = 0;

		/// \brief Pure virtual function that is called when the GameState is
		///		resume due to all GameStates above being pop'd.
		virtual void OnResume() = 0;

		/// \brief Pure virtual function that is called when the GameLoop wants
		///		to update the GameState.
		///	\details This should calculate all the game logic that is required
		///		for the specific GameState, e.g. updating the objects, advancing
		///		timers etc.
		/// \param [in] dt	Delta time since last frame in seconds.
		virtual void Update(float dt) = 0;

		/// \brief Pure virtual functions that is called when the GameLoop wants
		///		to draw everything.
		/// \details Use this function to specifiy what needs to be drawn: the
		///		map, certain menus etc.
		virtual void Draw(sf::RenderWindow& win) = 0;

		/// \brief Tells the StateMachine wether the GameState is to be pop'd
		///
		/// The standard implementation returns m_finished, which can be set to
		/// true by the specific GameState implementation. Don't forget to set
		/// it to false in the constructor :)
		virtual bool IsFinished()
		{
			return m_finished;
		}

	protected:
		bool m_finished; ///< set to true if the GameState is finished
		GameState* m_previousState;
	};
}