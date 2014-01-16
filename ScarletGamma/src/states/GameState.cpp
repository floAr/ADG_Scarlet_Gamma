#include "GameState.hpp"
#include "sfutils\View.hpp"

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

	bool GameState::GuiHandleEvent( sf::Event& event )
	{
		bool result = false;
		if (m_currentGui)
		{
			// Update HUD without zoom and move
			sf::RenderWindow* win = m_currentGui->getWindow();
			sf::View backup = sfUtils::View::SetDefault(win);
			result = m_currentGui->handleEvent(event);
			win->setView(backup);
		}
		return result;
	}

	void GameState::GuiDraw()
	{
		if (m_currentGui)
		{
			// Draw HUD without zoom and move
			sf::RenderWindow* win = m_currentGui->getWindow();
			sf::View backup = sfUtils::View::SetDefault(win);
			m_currentGui->draw();
			win->setView(backup);
		}
	}

} // namespace States

