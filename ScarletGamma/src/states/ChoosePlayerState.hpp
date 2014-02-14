#pragma once

#include "GameState.hpp"
#include <TGUI/TGUI.hpp>
#include "Prerequisites.hpp"

namespace States
{
	/// \brief After connecting to the server the player must be chosen / created
	class ChoosePlayerState : public GameState
	{
	public:
		/// \param [in] _id The network determined player id. This id will be set
		///		in the player after choice is made.
		ChoosePlayerState(Core::PlayerID _id);

		virtual void Draw(sf::RenderWindow& win) override;

		/// \brief If a player was created start the game
		virtual void OnResume() override;

	private:
		Core::PlayerID m_playerID;
		void ChoosePlayer(const tgui::Callback& _callback);
		void CreatePlayer();

		tgui::Gui   m_gui;

		/// \brief If a new player is created the object is stored otherwise
		///		it is 0xffffffff.
		Core::ObjectID m_newPlayer;
	};
}