#pragma once

#include "states/GameState.hpp"
#include "utils/Falloff.hpp"
#include "Prerequisites.hpp"

namespace States
{
	class MasterState : public GameState
	{
	public:
		MasterState() : m_zoom(Utils::Falloff::FT_QUADRATIC, 0.75f, 0.05f), m_selected(nullptr), m_player(nullptr) {}
		virtual void OnBegin();
		virtual void OnEnd();
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

		Core::Object* m_selected;	///< A reference to the selected object or nullptr
		Core::Object* m_player;		///< The one and only player object
		void DrawPathOverlay(sf::RenderWindow& win);
	};
}