#include "GameState.hpp"
#include "sfutils\View.hpp"
#include "Game.hpp"

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
			SetGuiView();
			result = m_currentGui->handleEvent(event);
			SetStateView();
		}
		return result;
	}

	void GameState::GuiDraw()
	{
		if (m_currentGui)
		{
			// Draw HUD without zoom and move
			SetGuiView();
			m_currentGui->draw();
			SetStateView();
		}
	}

	void GameState::AddPopCallback(std::function<void(GameState*)> callback)
	{
		m_popCallbacks.push_back(callback);
	}

	void GameState::NotifyPopCallback()
	{
		for (std::vector<std::function<void(GameState*)>>::iterator it = m_popCallbacks.begin();
			it != m_popCallbacks.end(); ++it)
		{
			// Call callback function
			(*it)(this);
		}
	}


	void GameState::SetGuiView()
	{
		m_stateView = g_Game->GetWindow().getView();
		m_guiView.setSize((float)g_Game->GetWindow().getSize().x, (float)g_Game->GetWindow().getSize().y);
		m_guiView.setCenter(m_guiView.getSize() * 0.5f);
		m_currentGui->getWindow()->setView(m_guiView);
	}

	void GameState::SetStateView()
	{
		m_currentGui->getWindow()->setView(m_stateView);
	}

} // namespace States