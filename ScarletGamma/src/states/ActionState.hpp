#pragma once

#include "GameState.hpp"
#include "../core/Object.hpp"
#include <TGUI/TGUI.hpp>

namespace States
{
	class ActionState : public GameState
	{
	public:
		ActionState();
		/// \brief Set object to target with action
		/// \details Sets the object to interact with, this will define the possible actions
		void SetTargetObject(Core::ObjectID);
		/// \brief Set object which is calling the action
		/// \details Sets the object which starts the action 
		void SetSourceObject(Core::ObjectID);
		/// \brief Set the GUI position
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
		void positionButton(tgui::Button::Ptr b, float angle, float radius);

		tgui::Gui m_gui;
		tgui::Button::Ptr m_defaultButton;	///< This is a preloaded button to increase RecalculateGUI performance by a high factor
		Core::ObjectID m_targetObject;		///< The object to be used
		Core::ObjectID m_sourceObject;		///< The object starting the action
		bool m_dirty;						///< Flag to mark a change in the GUI
		bool m_controlWasPressed;			///< Buffer for control key
		float m_screenX, m_screenY;			///< Position on screen
	};
}