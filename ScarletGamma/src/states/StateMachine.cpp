#include "StateMachine.hpp"
#include "IntroState.hpp"
#include "PlayerState.hpp"
#include "MainMenuState.hpp"
#include "MasterState.hpp"
#include "LaunchMasterState.hpp"
#include "LaunchPlayerState.hpp"
#include "ActionState.hpp"
#include "PromptState.hpp"
#include "DismissableDialogState.hpp"
#include "Game.hpp"
#include "network\ChatMessages.hpp"
#include "CreditState.h"

States::StateMachine::StateMachine() :
    m_gameState(0)
{
}

States::StateMachine::~StateMachine()
{
    // Remove all states from stack
    while (m_gameState)
    {
        m_gameState->OnEnd();
        GameState* oldState = m_gameState;
        m_gameState = m_gameState->GetPreviousState();
        delete oldState;
    }
}

States::GameState* States::StateMachine::PushGameState(States::GameState* newState)
{
    // If we have a new state, "push" it
    if (newState != 0)
    {
        // Remove finished GameStates before pushing a new one
        while (m_gameState && m_gameState->IsFinished())
            PopGameState();
        
        // Pause current game state
        if (m_gameState)
            m_gameState->OnPause();

        newState->SetPreviousState(m_gameState);
        newState->OnBegin();
        m_gameState = newState;
    }

    return newState;
}

States::GameState* States::StateMachine::PushGameState(States::GameStateType state)
{
    // Create initialized pointer
    GameState* newState = 0;

    // Fill the pointer, if the GameStateType is known
    switch(state)
    {
    case GST_INTRO:
        newState = new States::IntroState(std::string("media/acagamics.png"), 2);
        break;
    case GST_MAIN_MENU:
        newState = new States::MainMenuState();
        break;
    case GST_LAUNCH_MASTER:
        newState = new States::LaunchMasterState();
        break;
    case GST_LAUNCH_PLAYER:
        newState = new States::LaunchPlayerState();
        break;
	case GST_CREDITS:
		newState = new States::CreditState();
		break;
    case GST_PLAYER:
        // You mast create the player state your self - it requires input.
        assert(false);
        break;
    case GST_MASTER:
        // You mast create the master state your self - it requires input.
        assert(false);
        break;
    case GST_SELECTION:
        newState = new States::SelectionState();
        break;
    case GST_ACTION:
        newState = new States::ActionState();
        break;
    case GST_PROMPT:
        newState = new States::PromptState();
		break;
    case GST_DISMISS:
        newState = new States::DismissableDialogState("media/orb.png");
		break;
    }

    assert(newState && "StateMachine::PushGameState returned 0, didn't you add your enum?");

    // If we have a new state, "push" it
    if (newState != 0)
        PushGameState(newState);

    return newState;
}

void States::StateMachine::PopGameState()
{
    // Replace game state with predecessor - may be 0
    if (m_gameState)
    {
        GameState* oldState = m_gameState;
        m_gameState = m_gameState->GetPreviousState();

        oldState->OnEnd();
        oldState->NotifyPopCallback();
        delete oldState;

        if (m_gameState)
            m_gameState->OnResume();
    }
}

void States::StateMachine::Update(float dt)
{
	try {
		// Replace game state with predecessor - may be 0
		while (m_gameState && m_gameState->IsFinished())
		{
			PopGameState();
		}

		// Update, if there is a GameState left
		if (m_gameState)
			m_gameState->Update(dt);

		// Handle GUI callbacks
		GuiHandleCallbacks();
	} catch( std::string _msg ) {
		g_Game->AppendToChatLog( Network::ChatMsg( _msg, sf::Color::Red, true ) );
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler beim Update trat auf", sf::Color::Red, true ) );
	}
}

void States::StateMachine::Draw(sf::RenderWindow& win)
{
	try {
		// Draw GameState to window, if both exists
		if (win.isOpen() && m_gameState)
		{
			m_gameState->Draw(win);
		}
	} catch( std::string _msg ) {
		g_Game->AppendToChatLog( Network::ChatMsg( _msg, sf::Color::Red, true ) );
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler beim Zeichnen trat auf", sf::Color::Red, true ) );
	}
}

void States::StateMachine::TextEntered(char character, bool guiHandled)
{
	try {
		if (m_gameState)
			m_gameState->TextEntered(character, guiHandled);
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler bei Texteingabe trat auf", sf::Color::Red, true ) );
	}
}

void States::StateMachine::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
	try {
		if (m_gameState)
	        m_gameState->KeyPressed(key, guiHandled);
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler bei Eingabe trat auf", sf::Color::Red, true ) );
	}
}

void States::StateMachine::KeyReleased(sf::Event::KeyEvent& key, float time, bool guiHandled)
{
	try {
		if (m_gameState)
	        m_gameState->KeyReleased(key, time, guiHandled);
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler bei Eingabe trat auf", sf::Color::Red, true ) );
	}
}

void States::StateMachine::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled)
{
	try {
	    if (m_gameState)
	        m_gameState->MouseWheelMoved(wheel, guiHandled);
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler bei Eingabe trat auf", sf::Color::Red, true ) );
	}

}

void States::StateMachine::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled)
{
	try {
	    if (m_gameState)
	        m_gameState->MouseButtonPressed(button, tilePos, guiHandled);
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler bei Eingabe trat auf", sf::Color::Red, true ) );
	}

}

void States::StateMachine::MouseButtonReleased(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
                                               float time, bool guiHandled)
{
	try {
		if (m_gameState)
			m_gameState->MouseButtonReleased(button, tilePos, time, guiHandled);
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler bei Eingabe trat auf", sf::Color::Red, true ) );
	}
}

void States::StateMachine::MouseMoved(int deltaX, int deltaY, bool guiHandled)
{
	try {
		if (m_gameState)
			m_gameState->MouseMoved(deltaX, deltaY, guiHandled);
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler bei Eingabe trat auf", sf::Color::Red, true ) );
	}
}

bool States::StateMachine::GuiHandleEvent(sf::Event& event)
{
	try {
		if (m_gameState)
			return m_gameState->GuiHandleEvent(event);
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler bei GUI-Eingabe trat auf", sf::Color::Red, true ) );
	}

    return false;
}

void States::StateMachine::GuiHandleCallbacks()
{
	try {
		if (m_gameState)
			m_gameState->GuiHandleCallbacks();
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler in den GUI-Callbacks trat auf", sf::Color::Red, true ) );
	}
}

void States::StateMachine::GuiDraw()
{
	try {
		if (m_gameState)
			m_gameState->GuiDraw();
	} catch( ... ) {
		g_Game->AppendToChatLog( Network::ChatMsg( "Ein Fehler beim Zeichnen der GUI trat auf", sf::Color::Red, true ) );
	}
}


void States::StateMachine::Resize( const sf::Vector2f& _size )
{
    GameState* state = m_gameState;
    while( state )
    {
        state->Resize( _size );
        state = state->GetPreviousState();
    }
}