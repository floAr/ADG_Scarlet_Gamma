#pragma once

#include "GameState.hpp"
#include "../core/Object.hpp"
#include <TGUI/TGUI.hpp>
#include "interfaces/CircularMenu.hpp"

namespace States
{
	class ActionState : public GameState
	{
	public:
		ActionState();
		/// \brief Set object to target with action
		/// \details Sets the object to interact with, this will define the possible actions
		/// \param [in] target	The ObjectID of the target object
		void SetTargetObject(Core::ObjectID);
		/// \brief Set object which is calling the action
		/// \details Sets the object which starts the action
		/// \param [in] source	The ObjectID of the source object
		void SetSourceObject(Core::ObjectID);
		/// \brief Set the GUI position
		/// \param [in] x	x-position in pixels-coords
		/// \param [in] y	y-position in pixels-coords
		void SetPosition(int x,int y);
		/// \brief Triggers a recalculation of the GUI
		/// \details Let the GUI recalculate all the buttons and position
		void RecalculateGUI();
		virtual void OnBegin();
		virtual void OnEnd() {}
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt);
		virtual void GuiCallback(tgui::Callback&  args);
		virtual void Draw(sf::RenderWindow& win);
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			bool guiHandled) override;
	
	private:
		tgui::Gui m_gui;
		Interfaces::CircularMenu m_menu;
		Core::ObjectID m_targetObject;		///< The object to be used
		Core::ObjectID m_sourceObject;		///< The object starting the action
		bool m_dirty;						///< Flag to mark a change in the GUI
		bool m_controlWasPressed;			///< Buffer for control key
		float m_screenX, m_screenY;			///< Position on screen
	};
}