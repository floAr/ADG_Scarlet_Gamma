#pragma once

#include "GameState.hpp"

namespace States
{
	class MainMenuState : public GameState
	{
		virtual void OnBegin() {}
		virtual void OnEnd() {}
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt) {}
		virtual void Draw() {}
	};
}