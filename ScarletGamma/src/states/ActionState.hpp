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
		void SetTargetObject(Core::ObjectID);
		void SetSourceObject(Core::ObjectID);
		void SetPosition(int x,int y);
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