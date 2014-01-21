#pragma once

#include "Prerequisites.hpp"
#include "states/CommonState.hpp"
#include "interfaces/PropertyPanel.hpp"

namespace States
{
	class PlayerState : public CommonState
	{
	public:
		PlayerState( const std::string& _playerName, const sf::Color& _chatColor);
		virtual void OnBegin() override;
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			bool guiHandled) override;
		virtual void Resize(const sf::Vector2f& _size) override;

		/// \brief CommonState Update + Player view refreshed.
		virtual void Update( float _dt ) override;

	private:
		Core::Object* m_player;		///< The one and only player object

		Interfaces::PropertyPanel::Ptr m_playerView;	///< Showing the player properties
	};
}