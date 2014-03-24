#pragma once

#include "Prerequisites.hpp"
#include <SFML/Graphics.hpp>
#include <stack>

namespace States
{
    enum GameStateType { GST_INTRO, GST_MAIN_MENU, GST_LAUNCH_MASTER, GST_LAUNCH_PLAYER,
		GST_PLAYER, GST_MASTER, GST_CREDITS, GST_SELECTION, GST_ACTION, GST_PROMPT, GST_DISMISS };

    /// \brief Factory and manager for GameState implementations.
    /// This is used to keep track of the GameStates and initialize new ones.
    /// Their implementation remains hidden to the other classes.
    class StateMachine
    {
    public:
        /// \brief Creates a new StateMachine. Initializes the current GameState to 0.
        StateMachine();

        /// \brief Remove all remaining states
        ~StateMachine();

        /// \brief Creates the specified GameState and pushes it onto the stack.
        /// \detail To remove a GameState, the state has to mark itself as
        ///		finished. The game then returns to the previous GameState or
        ///		quits if it was the last one.
        /// \param [in] state  Identifies the state to be pushed
        GameState* PushGameState(GameStateType state);

        /// \brief Pushes an existing GameState onto the stack.
        /// \detail To remove a GameState, the state has to mark itself as
        ///		finished. The game then returns to the previous GameState or
        ///		quits if it was the last one.
        /// \param [in] state  Identifies the state to be pushed
        GameState* PushGameState(GameState* state);

        /// \brief Pops the topmost gamestate from the stack.
        void PopGameState();

        /// \brief Used for quitting the game when the last GameState has ended.
        /// \return true if at least one state is left, false otherwise
        inline bool HasStates()
        {
            return m_gameState != 0;
        }

        /// \brief Return a pointer to the current GameState
        inline GameState* GetGameState()
        {
            return m_gameState;
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
        /// \param [in] character   ASCII character that was entered.
        /// \param [in] guiHandled  Wether the GUI already used the event
        void TextEntered(char character, bool guiHandled);

        /// \brief Gets called by the InputHandler when any key is pressed.
        /// \param [in] key         SFML key event that contains all required information.
        /// \param [in] guiHandled  Wether the GUI already used the event
        void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled);

        /// \brief Gets called by the InputHandler when any key is released.
        /// \param [in] key         SFML key event that contains all required information.
        /// \param [in] time        How long the key was pressed, in seconds.
        /// \param [in] guiHandled  Wether the GUI already used the event
        void KeyReleased(sf::Event::KeyEvent& key, float time, bool guiHandled);


        //----------------------------------------------------------------------
        // MOUSE EVENTS

        /// \brief Gets called when the mouse wheel is moved.
        /// \param [in] wheel       SFML wheel event that contains all required information.
        /// \param [in] guiHandled  Wether the GUI already used the event
        void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled);

        /// \brief Gets called when a mouse button is pressed.
        /// \details Has the same internal logic as KeyPressed.
        /// \param [in] button      SFML button event that contains all required information.
        /// \param [in] tilePos     In-game tile that the user clicked on. Cast to float if required.
        /// \param [in] guiHandled  Wether the GUI already used the event
        void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled);

        /// \brief Gets called when a mouse button is released.
        /// \param [in] button      SFML button event that contains all required- information.
        /// \param [in] tilePos     In-game tile that the user clicked on. Cast to float if required.
        /// \param [in] time        How long the button was pressed, in seconds.
        /// \param [in] guiHandled  Wether the GUI already used the event
        void MouseButtonReleased(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, float time, bool guiHandled);

        /// \brief Gets called when the mouse is moved.
        /// \param [in] wheel       SFML move event that contains all required information
        /// \param [in] guiHandled  Wether the GUI already used the event.
        void MouseMoved(int deltaX, int deltaY, bool guiHandled);


        //----------------------------------------------------------------------
        // GUI STUFF

        /// \brief Forwards an event to the GUI to be handled.
        /// \param [in] event  Event information from SFML.
		/// \return false if the event was ignored.
		bool GuiHandleEvent(sf::Event& event);

		/// \brief Handles all GUI callbacks that have occurred since the last frame-
        void GuiHandleCallbacks();

        /// \brief Draws the current GUI.
        void GuiDraw();

		/// \brief Call the resize event for all game states
		void Resize( const sf::Vector2f& _size );

    private:
        /// \brief Pointer to the current GameState. The previous state is known
        ///		by the current GameState.
        GameState* m_gameState;
    };
}