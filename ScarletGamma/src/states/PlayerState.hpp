#pragma once

#include "Prerequisites.hpp"
#include "states/CommonState.hpp"
#include "interfaces/PropertyPanel.hpp"
#include <unordered_map>

namespace States
{
	class PlayerState : public CommonState
	{
	public:
		PlayerState( const std::string& _playerName, const sf::Color& _chatColor, Core::PlayerID _id );
		virtual void OnBegin() override;
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void MouseMoved(int deltaX, int deltaY, bool guiHandled) override;
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			bool guiHandled) override;
		/// \brief Update gui elements + common state actions
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
		virtual void Resize(const sf::Vector2f& _size) override;
		virtual Core::Map* GetCurrentMap() override;

		/// \brief CommonState Update + Player view refreshed.
		virtual void Update( float _dt ) override;

		virtual void BeginCombat(Core::ObjectID _object) override;

	private:
		Core::PlayerID m_playerID;							///< The players id (set by socket index)
		Core::Object* m_player;								///< The one and only player object
		Core::Object* m_focus;								///< The camera is tracking this object
		std::unordered_map<int,Core::ObjectID> m_hotkeys;	///< Defined hotkeys of this client //TODO: maybe save hotkeys as properties

		Interfaces::PropertyPanel::Ptr m_playerView;	///< Showing the player properties

		/// \brief This method is given to the renderer to check the
		///		visibility of a tile for the player
		float CheckTileVisibility(Core::Map& _map, sf::Vector2i& _tilePos, sf::Vector2f& _playerPos) const;

		/// \brief Set the current view to another objects position
		/// \details Try to append the current view onto another object, if player does not exist nothing happens
		/// \param [in] hotkey	pressed hotkey of the player
		void SetViewToHotkey(const int hotkey);

		/// \brief Set the current view to another objects position
		/// \details Try to append the current view onto another object, if player does not exist nothing happens
		/// \param [in] object	object to view
		void SetViewToObject(Core::Object* object );



		/// \brief Bind an object to the hotkey
		/// \details Save the currently focussed object with the pressed hotkey
		/// \param [in] hotkey	pressed hotkey of the player
		/// \param [in] objectID	object to remember
		void SetHotkeyToObject(const int hotkey, Core::ObjectID objectID);
	};
}