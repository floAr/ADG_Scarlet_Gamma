#include "DismissableDialogState.hpp"
#include "utils/Content.hpp"
#include "Game.hpp"

namespace States{

	DismissableDialogState::DismissableDialogState():m_isMinimized(false),m_orb(Content::Instance()->LoadTexture("media/orb.png")),m_minimize(Content::Instance()->LoadTexture("media/orb_diceroll.png")){
		m_minimize.setPosition((sf::Vector2f)g_Game->GetWindow().mapCoordsToPixel(sf::Vector2f(0.9f,0.9f)));
		m_minimize.setScale(sf::Vector2f(0.15f,0.15f));
		m_orb.setScale(sf::Vector2f(0.25f,0.25f));
	}

	void DismissableDialogState::Update(float dt) {
		if(m_isMinimized){//break this chain if minimized
			m_previousState->Update(dt);
			return;
		}
		GameState::Update(dt);
	}

	void DismissableDialogState::Draw(sf::RenderWindow& win) {
		if(m_isMinimized){//break this chain if minimized
			m_previousState->Draw(win);// draw prev state
			win.draw(m_orb); //overlay orb
			return;
		}
		GameState::Draw(win);
		win.draw(m_minimize); //draw minimize on top
	}

	void DismissableDialogState::MouseButtonReleased(sf::Event::MouseButtonEvent& button,
		sf::Vector2f& tilePos, float time, bool guiHandled) { 
			if(guiHandled)
				return;
			sf::FloatRect bounds;
			if(m_isMinimized) //currently minimized
			{
				bounds=m_minimize.getGlobalBounds();
			}
			else //currently open
			{
				bounds=m_minimize.getGlobalBounds();
			}

			if(button.x > bounds.left && 
				button.x < bounds.left+bounds.width && 
				button.y > bounds.top && 
				button.y < bounds.top+bounds.height) //click on sprite
			{
				m_isMinimized=!m_isMinimized;
			}
			else //no orb was clicked -> pass click down
				m_previousState->MouseButtonReleased(button,tilePos,time,guiHandled);
	}
}