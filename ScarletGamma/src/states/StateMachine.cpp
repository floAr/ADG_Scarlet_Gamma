#include "StateMachine.hpp"
#include "IntroState.hpp"
#include "PlayerState.hpp"
#include "MainMenuState.hpp"
#include "MasterState.hpp"
#include "LaunchMasterState.hpp"
#include "LaunchPlayerState.hpp"
#include "ActionState.hpp"
#include "PromptState.hpp"

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
        m_gameState->OnEnd();
        m_gameState->NotifyPopCallback();
        GameState* oldState = m_gameState;
        m_gameState = m_gameState->GetPreviousState();
        delete oldState;
        if (m_gameState)
            m_gameState->OnResume();
    }
}

void States::StateMachine::Update(float dt)
{
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
}

void States::StateMachine::Draw(sf::RenderWindow& win)
{
    // Draw GameState to window, if both exists
    if (win.isOpen() && m_gameState)
    {
        m_gameState->Draw(win);
    }
}

void States::StateMachine::TextEntered(char character, bool guiHandled)
{
    if (m_gameState)
        m_gameState->TextEntered(character, guiHandled);
}

void States::StateMachine::KeyPressed(sf::Event::KeyEvent& key, bool guiHandled)
{
    if (m_gameState)
        m_gameState->KeyPressed(key, guiHandled);
}

void States::StateMachine::KeyReleased(sf::Event::KeyEvent& key, float time, bool guiHandled)
{
    if (m_gameState)
        m_gameState->KeyReleased(key, time, guiHandled);
}

void States::StateMachine::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel, bool guiHandled)
{
    if (m_gameState)
        m_gameState->MouseWheelMoved(wheel, guiHandled);
}

void States::StateMachine::MouseButtonPressed(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos, bool guiHandled)
{
    if (m_gameState)
        m_gameState->MouseButtonPressed(button, tilePos, guiHandled);
}

void States::StateMachine::MouseButtonReleased(sf::Event::MouseButtonEvent& button, sf::Vector2f& tilePos,
                                               float time, bool guiHandled)
{
    if (m_gameState)
        m_gameState->MouseButtonReleased(button, tilePos, time, guiHandled);
}

void States::StateMachine::MouseMoved(int deltaX, int deltaY, bool guiHandled)
{
    if (m_gameState)
        m_gameState->MouseMoved(deltaX, deltaY, guiHandled);
}

bool States::StateMachine::GuiHandleEvent(sf::Event& event)
{
    if (m_gameState)
        return m_gameState->GuiHandleEvent(event);
    return false;
}

void States::StateMachine::GuiHandleCallbacks()
{
    if (m_gameState)
        m_gameState->GuiHandleCallbacks();
}

void States::StateMachine::GuiDraw()
{
    if (m_gameState)
        m_gameState->GuiDraw();
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