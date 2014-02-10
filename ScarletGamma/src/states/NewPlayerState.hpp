#pragma once

#include "GameState.hpp"
#include <TGUI/TGUI.hpp>

namespace States
{
	class NewPlayerState : public GameState
	{
	public:
		/// \brief Creating a new player also sets the name in the given edit
		///		to communicate the new name.
		NewPlayerState(tgui::EditBox::Ptr _nameEdit);
		virtual void Draw(sf::RenderWindow& win) override;
		virtual void KeyPressed(sf::Event::KeyEvent& key, bool guiHandled) override;
		virtual void GuiCallback(tgui::Callback& callback) override;
	private:
		sf::Font	m_menuFont;
		tgui::Gui   m_gui;

		tgui::EditBox::Ptr m_nameOutputEdit;
	};
}