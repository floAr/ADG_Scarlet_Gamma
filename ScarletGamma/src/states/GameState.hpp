#pragma once

#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>

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

		virtual ~GameState() {}

		/// \brief Sets the previous GameState, i.e. the GameState that the
		///		StateMachine will return to when this one is finished.
		/// \details If this is set to 0, the StateMachine will assume that
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

		/// \brief Virtual function that is called when the GameState is
		///		first entered and pushed.
		virtual void OnBegin() {}

		/// \brief Virtual function that is called when the GameState is
		///		finally pop'd from the stack.
		virtual void OnEnd() {}

		/// \brief Virtual function that is called when the GameState is
		///		paused due to another GameState being pushed on top of it.
		virtual void OnPause() {}

		/// \brief Virtual function that is called when the GameState is
		///		resume due to all GameStates above being pop'd.
		virtual void OnResume() {}

		/// \brief Virtual function that is called when the GameLoop wants
		///		to update the GameState.
		///	\details This should calculate all the game logic that is required
		///		for the specific GameState, e.g. updating the objects, advancing
		///		timers etc.
		/// \param [in] dt	Delta time since last frame in seconds.
		virtual void Update(float dt) {}

		/// \brief Function that is called when the GameLoop wants to draw
		///		everything. Derived classes must call this by GameState::Draw.
		/// \details Use this function to specify what needs to be drawn: the
		///		map, certain menus etc.
		virtual void Draw(sf::RenderWindow& win) { GuiDraw(); }

		/// \brief Tells the StateMachine whether the GameState is to be pop'd
		///
		/// The standard implementation returns m_finished, which can be set to
		/// true by the specific GameState implementation. Don't forget to set
		/// it to false in the constructor :)
		virtual bool IsFinished()
		{
			return m_finished;
		}


		//----------------------------------------------------------------------
		// KEYBOARD EVENTS

		/// \brief Gets called by the InputHandler when any ASCII character was
		///		entered.
		/// \details You shouldn't call this function manually, except for
		///		cases where you really want to fake user input (e.g. buttons
		///		that insert special characters or something).
		/// \param [in] character   ASCII character that was entered.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void TextEntered(char character, bool guiHandled) { }

		/// \brief Gets called by the InputHandler when any key is pressed.
		/// \param [in] key         SFML key event that contains all required information.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) { }

		/// \brief Gets called by the InputHandler when any key is released.
		/// \param [in] key         SFML key event that contains all required information.
		/// \param [in] time        How long the key was pressed, in seconds.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void KeyReleased(sf::Event::KeyEvent& key, float time, bool guiHandled) { }


		//----------------------------------------------------------------------
		// MOUSE EVENTS

		/// \brief Gets called when the mouse wheel is moved.
		/// \param [in] wheel       SFML wheel event that contains all required information.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled) { }

		/// \brief Gets called when a mouse button is pressed.
		/// \details Has the same internal logic as KeyPressed.
		/// \param [in] button      SFML button event that contains all required information.
		/// \param [in] tilePos     In-game tile that the user clicked on. Cast to float if required.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button,
			sf::Vector2f& tilePos, bool guiHandled) { }

		/// \brief Gets called when a mouse button is released.
		/// \param [in] button      SFML button event that contains all required information.
		/// \param [in] tilePos     In-game tile that the user clicked on. Cast to float if required.
		/// \param [in] time        How long the button was pressed, in seconds.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void MouseButtonReleased(sf::Event::MouseButtonEvent& button,
			sf::Vector2f& tilePos, float time, bool guiHandled) { }

		/// \brief Gets called when the mouse is moved.
		/// \param [in] deltaX      Relative horizontal mouse movement since last frame.
		/// \param [in] deltaY      Relative vertical mouse movement since last frame.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void MouseMoved(int deltaX, int deltaY, bool guiHandled) { }

		//----------------------------------------------------------------------
		// GUI STUFF

		/// \brief Gets called when a GUI callback is triggered.
		/// \param [in] callback  TGUI callback information
		virtual void GuiCallback(tgui::Callback& callback) { }

		/// \brief Handles all GUI callbacks that have occurred since the last frame-
		void GuiHandleCallbacks();

		/// \brief Forwards an event to the GUI to be handled.
		/// \param [in] event  Event information from SFML.
		/// \return false if the event was ignored.
		virtual bool GuiHandleEvent(sf::Event& event);

		/// \brief Draws the current GUI.
		void GuiDraw();

		/// \brief Rescale or realign your GUI stuff.
		/// \param [in] _size New size of the window (size of view).
		virtual void Resize(const sf::Vector2f& _size) { }


		//----------------------------------------------------------------------
		// EVENT CALLBACK STUFF

		/// \brief Add a function to be notified before this GameState is popped.
		/// \param [in] callback  Use std::bind to pass the callback function
		void AddPopCallback(std::function<void(GameState*)> callback);

		/// \brief Notify all registered functions that this GameState is about
		///   to end. Should ONLY be called by the StateMachine!
		void NotifyPopCallback();

	protected:
		bool m_finished; ///< set to true if the GameState is finished
		GameState* m_previousState; ///< Pointer to previous state or null
		std::vector<std::function<void(GameState*)>> m_popCallbacks; ///< Functions to be notified before I'm gone


		/// \brief Sets the GUI to be rendered, updated and used to handle events.
		/// \param [in] gui  Pointer to the GUI instance.
		void SetGui(tgui::Gui* gui) { m_currentGui = gui ; }

	private:
		tgui::Gui* m_currentGui; ///< GUI to be rendered, updated and used to handle events.
	};
}