#pragma once

#include "GameState.hpp"
#include <TGUI/TGUI.hpp>
#include "Prerequisites.hpp"

namespace States
{
	class NewPlayerState : public GameState
	{
	public:
		/// \brief Creating a new player also sets the name in the given edit
		///		to communicate the new name.
		NewPlayerState(tgui::EditBox::Ptr _nameEdit, Core::ObjectID* _saveID);
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
	private:
		void Cancel();
		void Create();

		sf::Font	m_menuFont;
		tgui::Gui   m_gui;

		/// \brief Output of the name to the parent state
		tgui::EditBox::Ptr m_nameOutputEdit;
		/// \brief Output of the id to the parent state
		Core::ObjectID* m_newPlayer;

		// All the components which contain data
		tgui::EditBox::Ptr m_name;
	};
}