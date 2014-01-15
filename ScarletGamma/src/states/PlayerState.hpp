#pragma once

#include "Prerequisites.hpp"
#include "states/CommonState.hpp"
#include "graphics/EditList.hpp"

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

		/// \brief CommonState Update + Player view refreshed.
		virtual void Update( float _dt ) override;

	private:
		Core::Object* m_player;		///< The one and only player object

		Graphics::PropertyPanel::Ptr m_playerView;	///< Showing the player properties
	};
}