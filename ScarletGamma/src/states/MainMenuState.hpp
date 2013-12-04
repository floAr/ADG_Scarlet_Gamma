#pragma once

#include "GameState.hpp"

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
		virtual void KeyPressed(sf::Event::KeyEvent key);
	private:
		sf::Font	m_menuFont;
	};
}