#include "DismissableDialogState.hpp"
#include "utils/Content.hpp"
#include "Game.hpp"
#include "sfutils\View.hpp"

namespace States{

	DismissableDialogState::DismissableDialogState():m_isMinimized(false),m_orb(Content::Instance()->LoadTexture("media/orb.png")),m_minimize(Content::Instance()->LoadTexture("media/orb_diceroll.png")){
		auto vp  =g_Game->GetWindow().getView().getSize();			
		m_minimize.setPosition(sf::Vector2f(vp.x-m_minimize.getGlobalBounds().width,0));
		m_minimize.setScale(sf::Vector2f(0.15f,0.15f));
		m_orb.setScale(sf::Vector2f(0.25f,0.25f));
	}

	void DismissableDialogState::Update(float dt) {
		if(m_isMinimized){//break this chain if minimized
			m_previousState->Update(dt);
			return;
		}
		m_previousState->Update(dt);
		GameState::Update(dt);
	}

	void DismissableDialogState::Draw(sf::RenderWindow& win) {
		//update sprite 
		m_previousState->Draw(win);// draw prev state
		if(m_isMinimized){//break this chain if minimized
			win.draw(m_orb); //overlay orb
			return;
		}
		GameState::Draw(win);
		sf::View& backup=sfUtils::View::SetDefault(&win);
		win.draw(m_minimize); //draw minimize on top
		win.setView(backup);
	}

	void DismissableDialogState::MouseButtonReleased(sf::Event::MouseButtonEvent& button,
		sf::Vector2f& tilePos, float time, bool guiHandled) { 
			if(guiHandled)
				return;
			sf::FloatRect bounds;
			if(m_isMinimized) //currently minimized
			{
				bounds=m_orb.getGlobalBounds();
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

	void DismissableDialogState::Resize(const sf::Vector2f& _size) {
		if(m_isMinimized){
			m_previousState->Resize(_size);				
			return;
		}
		auto vp  =g_Game->GetWindow().getView().getSize();			
		m_minimize.setPosition(sf::Vector2f(vp.x-m_minimize.getGlobalBounds().width,0));
	}

}