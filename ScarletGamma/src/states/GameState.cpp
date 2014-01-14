#include "GameState.hpp"

namespace States {


	void GameState::GuiHandleCallbacks()
	{
		if (m_currentGui)
		{
			tgui::Callback callback;
			while (m_currentGui->pollCallback(callback))
				GuiCallback(callback);
		}
	}

	void GameState::GuiHandleEvent( sf::Event& event )
	{
		if (m_currentGui)
		{
			// Update HUD without zoom and move
			sf::RenderWindow* win = m_currentGui->getWindow();
			sf::View backup = win->getView();
			win->setView(win->getDefaultView());
			m_currentGui->handleEvent(event);
			win->setView(backup);
		}
	}

	void GameState::GuiDraw()
	{
		if (m_currentGui)
		{
			// Draw HUD without zoom and move
			sf::RenderWindow* win = m_currentGui->getWindow();
			sf::View backup = win->getView();
			win->setView(win->getDefaultView());
			m_currentGui->draw();
			win->setView(backup);
		}
	}

} // namespace States

