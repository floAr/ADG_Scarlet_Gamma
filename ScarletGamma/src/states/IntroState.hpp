#pragma once

#include "GameState.hpp"

namespace States
{
	class IntroState : public GameState
	{
	public:
		IntroState(std::string& filename, float duration);
		virtual void Update(float dt) override;
		virtual void Draw(sf::RenderWindow& win) override;
	private:
		sf::Texture	m_logoTexture;
		sf::Sprite	m_logoSprite;
		float		m_maxTime;
		float		m_totalTime;
	};
}