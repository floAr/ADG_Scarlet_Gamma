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
		sf::Sprite m_orb;		///< Sprite shown when minimized
		const static int ORB_WIDTH=35;	///< pixelsize of one Orb
		bool m_isMinimizeable;	///< Bool indicating if the state can be minimized

	public:
		/// \brief Stops this state from beeing minimizeable
		virtual void DisableMinimize();
		virtual void SetMinimized(bool _value);
		virtual void SetOrbSprite(const std::string& texture);

		/// \brief Creates a Zoom object, The GUI for chats.
		DismissableDialogState();	

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

				/// \brief Gets called when a mouse button is pressed.
		/// \details Has the same internal logic as KeyPressed.
		/// \param [in] button      SFML button event that contains all required information.
		/// \param [in] tilePos     In-game tile that the user clicked on. Cast to float if required.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button,
			sf::Vector2f& tilePos, bool guiHandled);


		// pass everything else downward when minimized
		virtual void TextEntered(char character, bool guiHandled){
			if(m_isMinimized){
				m_previousState->TextEntered(character,guiHandled);
				return;
			}
		}


		/// \brief Gets called by the InputHandler when any key is pressed.
		/// \param [in] key         SFML key event that contains all required information.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled){
			if(m_isMinimized){
				m_previousState->KeyPressed(key,guiHandled);	
				return;
			}
		}


		/// \brief Gets called by the InputHandler when any key is released.
		/// \param [in] key         SFML key event that contains all required information.
		/// \param [in] time        How long the key was pressed, in seconds.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void KeyReleased(sf::Event::KeyEvent& key, float time, bool guiHandled){
			if(m_isMinimized){
				m_previousState->KeyReleased(key,time,guiHandled);
				return;
			}
		}



		//----------------------------------------------------------------------
		// MOUSE EVENTS

		/// \brief Gets called when the mouse wheel is moved.
		/// \param [in] wheel       SFML wheel event that contains all required information.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled){
			if(m_isMinimized){
				m_previousState->MouseWheelMoved(wheel,guiHandled);
				return;
			}
		}



		/// \brief Gets called when a mouse button is released.
		/// \param [in] button      SFML button event that contains all required information.
		/// \param [in] tilePos     In-game tile that the user clicked on. Cast to float if required.
		/// \param [in] time        How long the button was pressed, in seconds.
		/// \param [in] guiHandled  Wether the GUI already used the event

		virtual void MouseButtonReleased(sf::Event::MouseButtonEvent& button,
			sf::Vector2f& tilePos,float time, bool guiHandled){
				if(m_isMinimized){
					m_previousState->MouseButtonReleased(button,tilePos,time,guiHandled);
					return;
				}
		}


		/// \brief Gets called when the mouse is moved.
		/// \param [in] deltaX      Relative horizontal mouse movement since last frame.
		/// \param [in] deltaY      Relative vertical mouse movement since last frame.
		/// \param [in] guiHandled  Wether the GUI already used the event
		virtual void MouseMoved(int deltaX, int deltaY, bool guiHandled){
			if(m_isMinimized){
				m_previousState->MouseMoved(deltaX,deltaY,guiHandled);
				return;
			}
		}


		virtual void GuiCallback(tgui::Callback& callback){
			if(m_isMinimized){
				m_previousState->GuiCallback(callback);
				return;
			}
		}


		/// \brief Forwards an event to the GUI to be handled.
		/// \param [in] event  Event information from SFML.
		/// \return false if the event was ignored.
		virtual bool GuiHandleEvent(sf::Event& event) override
		{
			if (m_isMinimized)
				return m_previousState->GuiHandleEvent(event);
			else
				return GameState::GuiHandleEvent(event);
		}

		virtual void Resize(const sf::Vector2f& _size);
		/// \brief adds a new minimized Orb
		static void AddOrb(sf::Sprite* _orb);

		/// \brief removes a minimized orb
		static void RemoveOrb(sf::Sprite* _orb);

	private:
		bool m_isMinimized;		///< Indicates wether the state is currently minimized
		sf::Sprite m_minimize;	///< Sprite to start the minimize process

		int m_orbID;			///< index of the minimized orb
		sf::Shader m_shader;	///< Shader for the blur effect

		sf::Sprite m_background; ///< Background image

		static std::vector<sf::Sprite*> ms_orbs;	///< List of currently minimized orbs

		static void RecalculateOrbPositions();
	};
}