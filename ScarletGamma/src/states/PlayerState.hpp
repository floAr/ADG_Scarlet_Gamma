#pragma once

#include "states/GameState.hpp"

namespace States
{
	class PlayerState : public GameState
	{
	public:
		virtual void OnBegin() {}
		virtual void OnEnd() {}
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt);
		virtual void Draw(sf::RenderWindow& win);
		virtual void MouseMoved(sf::Event::MouseMoveEvent& move);
	private:
		int m_mousePos[2];
	};
}