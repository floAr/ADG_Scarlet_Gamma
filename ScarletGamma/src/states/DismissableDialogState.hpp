#pragma once

#include "states/GameState.hpp"
#include "utils/Falloff.hpp"
#include "Prerequisites.hpp"

namespace States
{
	/// \brief A base class for master and player state to cover common actions.
	/// \details All implemented methods should be used form derived class if
	///		the set of actions should be used. Which actions are done by the
	///		implementation is stated by the method documentation.
	class DismissableDialogState : public GameState
	{
	protected:
		/// \brief Set the texture of the orb
		//	void SetOrbTexture(sf::Texture _orbTexture);
		sf::Sprite  m_orb;		  ///< Sprite shown when minimized
		const static int ORB_WIDTH = 51;	///< pixel size of one Orb
		bool m_isMinimizeable;	///< Bool indicating if the state can be minimized
		bool m_forceKeepAlive;	///< does not close the State after pressing a button, but minimizing it

	public:
		/// \brief Stops this state from beeing minimizeable
		virtual void DisableMinimize();
		virtual void SetMinimized(bool _value);
		virtual void SetKeepAlive(bool _value);
		virtual void SetOrbSprite(const std::string& texture);

		/// \brief Creates an orb in the global list
		DismissableDialogState(const std::string& _orbTexture);

		/// \brief Remove the orb from the list
		~DismissableDialogState();

		/// \brief Virtual function that is called when the GameLoop wants
		///		to update the GameState.
		///	\details This should calculate all the game logic that is required
		///		for the specific GameState, e.g. updating the objects, advancing
		///		timers etc.
		/// \param [in] dt	Delta time since last frame in seconds.
		virtual void Update(float dt);

		/// \brief Function that is called when the GameLoop wants to draw
		///		everything. Derived classes must call this by GameState::Draw.
		/// \details Use this function to specify what needs to be drawn: the
		///		map, certain menus etc.
		virtual void Draw(sf::RenderWindow& win);

        /// \brief Forwards pause, required for mouse cursor setting.
        virtual void OnPause() override
        {
            m_previousState->OnPause();
        }

        /// \brief Forwards resume, required for mouse cursor setting.
        virtual void OnResume() override
        {
            m_previousState->OnResume();
        }

		/// \brief Gets called when a mouse button is pressed.
		/// \details Has the same internal logic as KeyPressed.
		/// \param [in] button      SFML button event that contains all required information.
		/// \param [in] tilePos     In-game tile that the user clicked on. Cast to float if required.
		/// \param [in] guiHandled  Whether the GUI already used the event
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button,
			sf::Vector2f& tilePos, bool guiHandled) override;


		// pass everything else downward when minimized
		virtual void TextEntered(char character, bool guiHandled) override
		{
			if(m_isMinimized)
				m_previousState->TextEntered(character,guiHandled);
		}


		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override
		{
			if(m_isMinimized)
				m_previousState->KeyPressed(key,guiHandled);	
		}


		virtual void KeyReleased(sf::Event::KeyEvent& key, float time, bool guiHandled) override
		{
			if(m_isMinimized)
				m_previousState->KeyReleased(key,time,guiHandled);
		}



		//----------------------------------------------------------------------
		// MOUSE EVENTS
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled) override
		{
			if(m_isMinimized)
				m_previousState->MouseWheelMoved(wheel,guiHandled);
		}


		virtual void MouseButtonReleased(sf::Event::MouseButtonEvent& button,
			sf::Vector2f& tilePos,float time, bool guiHandled) override
		{
			if(m_isMinimized)
				m_previousState->MouseButtonReleased(button,tilePos,time,guiHandled);
		}


		virtual void MouseMoved(int deltaX, int deltaY, bool guiHandled) override
		{
			if(m_isMinimized)
				m_previousState->MouseMoved(deltaX,deltaY,guiHandled);
		}


		virtual void GuiCallback(tgui::Callback& callback) override
		{
			if(m_isMinimized)
				m_previousState->GuiCallback(callback);
		}


		/// \brief Forwards an event to the GUI to be handled.
		/// \param [in] event  Event information from SFML.
		/// \return false if the event was ignored.
		virtual bool GuiHandleEvent(sf::Event& event) override
		{
			if (m_isMinimized)
				return m_previousState->GuiHandleEvent(event);
			else {
				bool result = GameState::GuiHandleEvent(event);
				GameState::GuiHandleCallbacks();
				return result;
			}
		}

		virtual void Resize(const sf::Vector2f& _size);
		/// \brief adds a new minimized Orb
		static void AddOrb(sf::Sprite* _orb);

		/// \brief removes a minimized orb
		static void RemoveOrb(sf::Sprite* _orb);

	private:
		bool m_isMinimized;		///< Indicates whether the state is currently minimized

		int m_orbID;			///< index of the minimized orb
		sf::Shader m_shader;	///< Shader for the blur effect

		static std::vector<sf::Sprite*> m_orbs;			///< List of all currently existing orbs
		static DismissableDialogState* m_activeDDState;	///< The dismissable states are mutually exclusive -> minimize an other one if the current one is maximized

		static void RecalculateOrbPositions();
	};
}