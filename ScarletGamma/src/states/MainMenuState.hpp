#pragma once

#include "GameState.hpp"
#include <TGUI/TGUI.hpp>

namespace States
{
	class MainMenuState : public GameState
	{
	public:
		MainMenuState();
		virtual void Update(float dt) override;
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
		virtual void MouseMoved(int deltaX, int deltaY, bool guiHandled) override;
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled) override;
		virtual void GuiCallback(tgui::Callback& callback) override;
	private:
		sf::Font	m_menuFont;
		int			m_mousePos[3];
		tgui::Gui   m_gui;
		sf::Sprite	m_background;
	};
}