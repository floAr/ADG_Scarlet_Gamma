#pragma once

#include "states/GameState.hpp"
#include "utils/Falloff.hpp"

namespace States
{
	class PlayerState : public GameState
	{
	public:
		PlayerState() : m_zoom(Utils::Falloff::FT_QUADRATIC, 0.75f, 0.05f) {}
		virtual void OnBegin() {}
		virtual void OnEnd() {}
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt);
		virtual void Draw(sf::RenderWindow& win);
		virtual void MouseMoved(int deltaX, int deltaY);
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos);
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel);
	private:
		void ZoomView(float delta);
		Utils::Falloff m_zoom;
	};
}