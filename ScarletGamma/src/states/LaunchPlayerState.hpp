#pragma once

#include "GameState.hpp"
#include <TGUI/TGUI.hpp>
#include "Prerequisites.hpp"

namespace States
{
	class LaunchPlayerState : public GameState
	{
	public:
		LaunchPlayerState();
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
		virtual void GuiCallback(tgui::Callback& callback) override;

	private:
		sf::Font	m_menuFont;
		tgui::Gui   m_gui;
		sf::Sprite	m_background;
		sf::Sprite	m_logo;
	};
}