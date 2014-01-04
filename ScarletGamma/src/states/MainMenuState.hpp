#pragma once

#include "GameState.hpp"
#include <TGUI/TGUI.hpp>

namespace States
{
	class MainMenuState : public GameState
	{
	public:
		MainMenuState();
		virtual void OnBegin() {}
		virtual void OnEnd() {}
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt);
		virtual void Draw(sf::RenderWindow& win);
		virtual void KeyPressed(sf::Event::KeyEvent& key);
		virtual void MouseMoved(int deltaX, int deltaY);
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel);
		virtual void GuiCallback(tgui::Callback& callback);
	private:
		sf::Font	m_menuFont;
		int			m_mousePos[3];
		tgui::Gui   m_gui;
	};
}