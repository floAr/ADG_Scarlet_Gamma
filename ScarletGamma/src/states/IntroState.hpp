#pragma once

#include "GameState.hpp"

namespace States
{
	class IntroState : public GameState
	{
	public:
		IntroState(std::string& filename, float duration);
		virtual void OnBegin() {}
		virtual void OnEnd() {}
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Update(float dt);
		virtual void Draw(sf::RenderWindow& win);
	private:
		sf::Texture	m_logoTexture;
		sf::Sprite	m_logoSprite;
		float		m_maxTime;
		float		m_totalTime;
	};
}