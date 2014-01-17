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
		void SetTilePosition(int x, int y, float _screenX, float _screenY);
		void RecalculateGUI();
		virtual void OnBegin();
		virtual void OnEnd() {}
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt);
		virtual void GuiCallback(tgui::Callback&  args);
		virtual void Draw(sf::RenderWindow& win);
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos);

	private:
		void positionButton(tgui::Button::Ptr b, float angle, float radius);

		tgui::Gui m_gui;
		tgui::Button::Ptr m_defaultButton;	///< This is a preloaded button to increase RecalculateGUI performance by a height factor
		const Core::ObjectList* m_objects;	///< The object list reference for the current tile
		Core::ObjectList* m_alreadySelected;
		bool m_dirty;
		bool m_selected;
		float m_screenX, m_screenY;
	};
}