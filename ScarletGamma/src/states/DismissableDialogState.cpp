#include "DismissableDialogState.hpp"
#include "utils/Content.hpp"
#include "Game.hpp"
#include "sfutils\View.hpp"
#include <algorithm>

namespace States{

	std::vector<sf::Sprite*> DismissableDialogState::ms_orbs;

	DismissableDialogState::DismissableDialogState() : 
		m_isMinimized(false), m_orb(Content::Instance()->LoadTexture("media/orb.png")),
		m_minimize(Content::Instance()->LoadTexture("media/orb_minimize.png")),
		m_background(Content::Instance()->LoadTexture("media/prompt.jpg"))
	{
		m_shader.loadFromFile("media/Prompt.frag", sf::Shader::Fragment);

		//m_minimize.setScale(sf::Vector2f(0.15f,0.15f));
		float orbScale=DismissableDialogState::ORB_WIDTH/m_orb.getLocalBounds().width;
		m_orb.setScale(sf::Vector2f(orbScale,orbScale));

		float minimizeOrbScale=DismissableDialogState::ORB_WIDTH/m_minimize.getLocalBounds().width;
		m_minimize.setScale(sf::Vector2f(minimizeOrbScale,minimizeOrbScale));


		//m_isMinimized=true;
		Resize(g_Game->GetWindow().getView().getSize());
	}

	/*void DismissableDialogState::SetOrbTexture(sf::Texture _orbTexture){
		
		float orbScale=DismissableDialogState::ORB_WIDTH/m_orb.getLocalBounds().width;
		m_orb.setScale(sf::Vector2f(orbScale,orbScale));
	}*/

	void DismissableDialogState::Update(float dt)
	{
		m_previousState->Update(dt);
		if(m_isMinimized){//break this chain if minimized			
			return;
		}
		GameState::Update(dt);
	}

	void DismissableDialogState::Draw(sf::RenderWindow& win)
	{
		// update sprite 
		m_previousState->Draw(win);

		// break this chain if minimized
		if (m_isMinimized)
		{
			//m_previousState->Draw(win);
			SetGuiView();
			win.draw(m_orb); //overlay orb
			SetStateView();
			return;
		}

		// Draw previous state's output
		SetGuiView();
		sf::Texture screenBuffer;
		screenBuffer.create(win.getSize().x, win.getSize().y);
		screenBuffer.update(win);
		m_shader.setParameter("texture", sf::Shader::CurrentTexture);
		m_shader.setParameter("blur_radius_x", 1.0f / (float) win.getSize().x);
		m_shader.setParameter("blur_radius_y", 1.0f / (float) win.getSize().y);
		win.draw(sf::Sprite(screenBuffer), &m_shader);

		// Draw background
		win.draw(m_background);

		// Draw GUI
		SetStateView();
		GameState::Draw(win);

		// Draw the minimize button
		SetGuiView();
		if(m_isMinimizeable){
			win.draw(m_minimize); //draw minimize on top
		}

		SetStateView();
	}

	

	void DismissableDialogState::MouseButtonPressed(sf::Event::MouseButtonEvent& button,
	sf::Vector2f& tilePos, bool guiHandled)
	{ 
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
			button.y < bounds.top+bounds.height &&
			m_isMinimizeable) //click on sprite
		{
			m_isMinimized=!m_isMinimized;
			Resize(sf::Vector2f( (float) g_Game->GetWindow().getSize().x,
								 (float) g_Game->GetWindow().getSize().y));
			if(m_isMinimized)
				DismissableDialogState::AddOrb(&m_orb);
			else
				DismissableDialogState::RemoveOrb(&m_orb);
		}
		else //no orb was clicked -> pass click down
			if(m_isMinimized)
				m_previousState->MouseButtonPressed(button,tilePos,guiHandled);
	}


	void DismissableDialogState::Resize(const sf::Vector2f& _size)
	{
		if (m_isMinimized)
		{
			RecalculateOrbPositions();
			m_previousState->Resize(_size);				
			return;
		}

		m_background.setPosition(_size.x / 2.0f - m_background.getGlobalBounds().width / 2.0f, 60);
		m_minimize.setPosition(sf::Vector2f(std::min(_size.x - m_minimize.getGlobalBounds().width - 5, m_background.getGlobalBounds().left +
			m_background.getGlobalBounds().width - m_minimize.getGlobalBounds().width - 5), m_background.getGlobalBounds().top + 5));
	}

	void DismissableDialogState::AddOrb(sf::Sprite* _orb)
	{
		DismissableDialogState::ms_orbs.push_back(_orb);
		DismissableDialogState::RecalculateOrbPositions();
	}

	void DismissableDialogState::RemoveOrb(sf::Sprite* _orb)
	{

		auto it=std::find(DismissableDialogState::ms_orbs.begin(),DismissableDialogState::ms_orbs.end(),_orb);
		DismissableDialogState::ms_orbs.erase(it);
		RecalculateOrbPositions();
	}

	void DismissableDialogState::RecalculateOrbPositions()
	{
		int orbCount = DismissableDialogState::ms_orbs.size();
		float widthPerOrb=DismissableDialogState::ORB_WIDTH+2;
		float startPoint=g_Game->GetWindow().getSize().x/2-(widthPerOrb*orbCount/2);
		float yPoint=(float)g_Game->GetWindow().getSize().y-DismissableDialogState::ORB_WIDTH-5;
		int i;
		for(i=0;i<orbCount;i++)
		{
			ms_orbs[i]->setPosition(startPoint,yPoint);
			startPoint+=widthPerOrb;
		}
	}

	void DismissableDialogState::DisableMinimize(){
		m_isMinimizeable = false;
		m_isMinimized = false;
	}



}