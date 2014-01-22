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
		virtual void MouseMoved(int deltaX, int deltaY, bool guiHandled) override;
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			bool guiHandled) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
		virtual void Resize(const sf::Vector2f& _size) override;

		/// \brief CommonState Update + Player view refreshed.
		virtual void Update( float _dt ) override;

	private:
		Core::Object* m_player;		///< The one and only player object
		Core::Object* m_focus;		///< The camera is tracking this object

		Interfaces::PropertyPanel::Ptr m_playerView;	///< Showing the player properties

		/// \brief This method is given to the renderer to check the
		///		visibility of a tile for the player
		float CheckTileVisibility(Core::Map& _map, sf::Vector2i& _tilePos, sf::Vector2f& _playerPos) const;
	};
}