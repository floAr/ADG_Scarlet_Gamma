#include "GameState.hpp"
#include "sfutils\View.hpp"
#include "Game.hpp"
#include "utils\Clipboard.hpp"
#include "events\InputHandler.hpp"

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

	void GameState::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
	{
		// Handle copy & paste
		if( key.code == sf::Keyboard::C && Events::InputHandler::IsControlPressed() )
		{
			sf::String text = Copy();
			if( !text.isEmpty() )
				Utils::Clipboard::Instance()->SetClipboardText(text.toAnsiString().c_str());
		} else if( key.code == sf::Keyboard::V && Events::InputHandler::IsControlPressed() )
		{
			Paste( Utils::Clipboard::Instance()->GetClipboardText() );
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
		g_Game->GetWindow().setView(m_guiView);
	}

	void GameState::SetStateView()
	{
		g_Game->GetWindow().setView(m_stateView);
	}


	tgui::Widget::Ptr GameState::GetFocusedElement()
	{
		auto widgets = &m_currentGui->getWidgets();
		for( int i=0; i<(int)widgets->size(); ++i )
		{
			if( (*widgets)[i]->isFocused() )
			{
				// Test if the element is a container and search must go on...
				tgui::Container* container = dynamic_cast<tgui::Container*>((*widgets)[i].get());
				if( !container ) return (*widgets)[i];
				else {
					// "restart recursive"
					widgets = &container->getWidgets();
					i = -1;
				}
			}
		}

		return nullptr;
	}

	void GameState::Paste(const sf::String& _text)
	{
		// Try to get the focused edit
		tgui::Widget::Ptr widget = GetFocusedElement();
		tgui::EditBox* edit = dynamic_cast<tgui::EditBox*>(widget.get());
		if( edit )
		{
			// Emulate entering the whole text
			for( size_t i=0; i<_text.getSize(); ++i )
				edit->textEntered( _text[i] );
		}
	}

	sf::String GameState::Copy()
	{
		// Try to get the focused edit
		tgui::Widget::Ptr widget = GetFocusedElement();
		tgui::EditBox* edit = dynamic_cast<tgui::EditBox*>(widget.get());
		if( edit )
		{
			return edit->getSelectedText();
		}

		return "";
	}

} // namespace States