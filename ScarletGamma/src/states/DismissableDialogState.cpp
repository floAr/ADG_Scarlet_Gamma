#include "DismissableDialogState.hpp"
#include "utils/Content.hpp"
#include "Game.hpp"
#include "sfutils\View.hpp"
#include <algorithm>

namespace States{

	std::vector<sf::Sprite*> DismissableDialogState::m_orbs;
	DismissableDialogState* DismissableDialogState::m_activeDDState = nullptr;

	DismissableDialogState::DismissableDialogState(const std::string& _orbTexture) : 
		m_isMinimized(false), m_orb(Content::Instance()->LoadTexture(_orbTexture)),
		m_forceKeepAlive(false),
		m_isMinimizeable(true)
	{
		m_shader.loadFromFile("media/Prompt.frag", sf::Shader::Fragment);

		float orbScale = DismissableDialogState::ORB_WIDTH/m_orb.getLocalBounds().width;
		m_orb.setScale(sf::Vector2f(orbScale,orbScale));

		DismissableDialogState::AddOrb(&m_orb);

		// Make sure this is the only maximized state
		SetMinimized( false );
	}

	DismissableDialogState::~DismissableDialogState()
	{
		// Remove the orb from the global list
		DismissableDialogState::RemoveOrb(&m_orb);

		m_activeDDState = nullptr;
	}

	void DismissableDialogState::SetMinimized(bool _value)
    {
		m_isMinimized = _value;

		if( !_value )
		{
			// Make sure this is the only maximized state
			if( m_activeDDState && m_activeDDState != this )
				m_activeDDState->SetMinimized( true );
			m_activeDDState = this;
		} else m_activeDDState = nullptr;
	}

	void DismissableDialogState::SetOrbSprite(const std::string& texture)
    {
		sf::Vector2f tempPos = m_orb.getPosition();
		m_orb.setTexture( Content::Instance()->LoadTexture(texture) );
	}

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
		m_previousState->Draw(win);

		// Depending on the stack position things get buggy.
		// First all "background" orbs must be drawn, then the overlay shader...
		if( m_activeDDState && m_activeDDState == this )
		{
			// Draw them all
			SetGuiView();
			for( size_t i = 0; i < m_orbs.size(); ++i )
				win.draw(*m_orbs[i]);
			SetStateView();
		}

		// break this chain if minimized
		if (m_isMinimized)
		{
			// Was drawn from active state?
			if( !m_activeDDState )
			{
				SetGuiView();
				win.draw(m_orb); // overlay orb
				SetStateView();
			}
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
	//	win.draw(m_background);

		// Draw GUI
		SetStateView();
		GameState::Draw(win);

		// Draw the minimize button
		SetGuiView();
		if(m_isMinimizeable){
			win.draw(m_orb);
		}

		SetStateView();
	}



	void DismissableDialogState::MouseButtonPressed(sf::Event::MouseButtonEvent& button,
		sf::Vector2f& tilePos, bool guiHandled)
	{ 
		// These orbs overlay the Gui!
		//if(guiHandled)
		//	return;

		// If nobody is maximized or this one is maximized..
		if( !m_activeDDState || m_activeDDState == this )
		{
			// Test if the orb was clicked
			sf::FloatRect bounds;
			bounds = m_orb.getGlobalBounds();

			if(button.x > bounds.left && 
				button.x < bounds.left+bounds.width && 
				button.y > bounds.top && 
				button.y < bounds.top+bounds.height &&
				m_isMinimizeable) //click on sprite
			{
				SetMinimized( !m_isMinimized );
				Resize(sf::Vector2f( (float) g_Game->GetWindow().getSize().x,
					(float) g_Game->GetWindow().getSize().y));
			} else //no orb was clicked -> pass click down
				if(m_isMinimized)
					m_previousState->MouseButtonPressed(button,tilePos,guiHandled);
		} else
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
	}

	void DismissableDialogState::AddOrb(sf::Sprite* _orb)
	{
		DismissableDialogState::m_orbs.push_back(_orb);
		DismissableDialogState::RecalculateOrbPositions();
	}

	void DismissableDialogState::RemoveOrb(sf::Sprite* _orb)
	{

		auto it=std::find(DismissableDialogState::m_orbs.begin(),DismissableDialogState::m_orbs.end(),_orb);
		DismissableDialogState::m_orbs.erase(it);
		RecalculateOrbPositions();
	}

	void DismissableDialogState::RecalculateOrbPositions()
	{
		int orbCount = DismissableDialogState::m_orbs.size();
		float widthPerOrb = DismissableDialogState::ORB_WIDTH+2;
		float startPoint = g_Game->GetWindow().getSize().x/2 - (widthPerOrb*orbCount/2);
		float yPoint = (float)g_Game->GetWindow().getSize().y - DismissableDialogState::ORB_WIDTH-5;
		for(int i=0; i<orbCount; i++)
		{
			m_orbs[i]->setPosition(startPoint,yPoint);
			startPoint+=widthPerOrb;
		}
	}

	void DismissableDialogState::DisableMinimize()
	{
		m_isMinimizeable = false;
		SetMinimized( false );
	}

	void DismissableDialogState::SetKeepAlive(bool _value)
	{
		m_forceKeepAlive = _value;
	}



}