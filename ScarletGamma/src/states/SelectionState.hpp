#pragma once

#include "GameState.hpp"
#include "../core/Object.hpp"
#include <TGUI/TGUI.hpp>

namespace States
{
	class SelectionState : public GameState
	{
	public:
		SelectionState();
		void SetTilePosition(int x, int y);
		void RecalculateGUI();
		virtual void OnBegin() override;
		virtual void Update(float dt) override;
		virtual void GuiCallback(tgui::Callback&  args) override;
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			bool guiHandled) override;

	private:
		void positionButton(tgui::Button::Ptr b, float angle, float radius);

		tgui::Gui m_gui;
		tgui::Button::Ptr m_defaultButton;	///< This is a preloaded button to increase RecalculateGUI performance by a height factor
		Core::ObjectList m_objects;			///< A copy of the object list for the current tile.
		bool m_dirty;						///< Flag to mark a change in the GUI
		bool m_controlWasPressed;			///< Buffer for control key
		unsigned int m_screenX, m_screenY;	///< Position on screen
	};
}