#pragma once

#include "GameState.hpp"
#include <TGUI/TGUI.hpp>

namespace States
{
	class LaunchPlayerState : public GameState
	{
	public:
		LaunchPlayerState();
		virtual void OnBegin()			{}
		virtual void OnEnd()			{}
		virtual void OnPause()			{}
		virtual void OnResume()			{}
		virtual void Update(float dt)	{}
		virtual void Draw(sf::RenderWindow& win);
		virtual void KeyPressed(sf::Event::KeyEvent& key);
		virtual void MouseMoved(int deltaX, int deltaY)					{}
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel)	{}
		virtual void GuiCallback(tgui::Callback& callback);
	private:
		sf::Font	m_menuFont;
		tgui::Gui   m_gui;
	};
}