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
		void SetObject(Core::ObjectID);
		void RecalculateGUI();
		virtual void OnBegin();
		virtual void OnEnd() {}
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt);
		virtual void GuiCallback(tgui::Callback&  args);
		virtual void Draw(sf::RenderWindow& win);
	
	private:
		void positionButton(tgui::Button::Ptr b, float angle, float radius);

		tgui::Gui m_gui;
		tgui::Button::Ptr m_defaultButton;	///< This is a preloaded button to increase RecalculateGUI performance by a height factor
		Core::ObjectID m_object;		///< The object to be used
		bool m_dirty;						///< Flag to mark a change in the GUI
		bool m_controlWasPressed;			///< Buffer for control key
		float m_screenX, m_screenY;			///< Position on screen
	};
}