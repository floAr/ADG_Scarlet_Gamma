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
		PlayerState( Core::ObjectID _player );

		virtual void OnBegin() override;
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			bool guiHandled) override;
		virtual void MouseButtonReleased(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
			float time, bool guiHandled) override;
		/// \brief Update gui elements + common state actions
		virtual void MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
		virtual void Resize(const sf::Vector2f& _size) override;
		virtual Core::Map* GetCurrentMap() override;
		virtual void CreateDiceRollState();

		/// \brief CommonState Update + Player view refreshed.
		virtual void Update( float _dt ) override;

		/// \brief Starts combat for an object.
		virtual void CreateCombat(Core::ObjectID _object) override;

		/// \brief Checks whether an object is owned by the player.
		virtual bool OwnsObject(Core::ObjectID _object) override;

		/// \brief Set the current view to another objects position
		/// \details Append the current view onto another object
		/// \param [in] object	object to view
		void FocusObject(Core::Object* _object);

		/// \brief Show the examination gui for the given object.
		void ExamineObject(Core::ObjectID _object);

	private:
		Core::PlayerID m_playerID;							///< The players id (set by socket index)
		Core::Object* m_player;								///< The one and only player object
		Core::ObjectID m_playerObjectID;					///< Persistent memory with the id required to show the characterstate.
		Core::Object* m_focus;								///< The camera is tracking this object
		std::unordered_map<int,Core::ObjectID> m_hotkeys;	///< Defined hotkeys of this client //TODO: maybe save hotkeys as properties

		Interfaces::PropertyPanel::Ptr m_playerView;		///< Showing the player properties
		Interfaces::PropertyPanel::Ptr m_observerView;		///< Showing something the player is observing

		/// \brief This method is given to the renderer to check the
		///		visibility of a tile for the player
		float CheckTileVisibility(Core::Map& _map, sf::Vector2i& _tilePos, sf::Vector2f& _playerPos) const;

		/// \brief Set the current view to another objects position
		/// \details Try to append the current view onto another object, if player does not exist nothing happens
		/// \param [in] hotkey	pressed hotkey of the player
		void SetViewToHotkey(const int hotkey);


		/// \brief Bind an object to the hotkey
		/// \details Save the currently focussed object with the pressed hotkey
		/// \param [in] hotkey	pressed hotkey of the player
		/// \param [in] objectID	object to remember
		void SetHotkeyToObject(const int hotkey, Core::ObjectID objectID);

		void RollDice(std::string& _value);
	};
}