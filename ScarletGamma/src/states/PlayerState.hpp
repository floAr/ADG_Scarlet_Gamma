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
	};
}