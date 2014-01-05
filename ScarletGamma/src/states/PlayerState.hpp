#pragma once

#include "Prerequisites.hpp"
#include "states/CommonState.hpp"

namespace States
{
	class PlayerState : public CommonState
	{
	public:
		PlayerState( const std::string& _playerName, const sf::Color& _chatColor);
		virtual void OnBegin();
		virtual void OnPause() {}
		virtual void OnResume() {}
		virtual void Draw(sf::RenderWindow& win);
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos);
	private:
		Core::Object* m_player;		///< The one and only player object
	};
}