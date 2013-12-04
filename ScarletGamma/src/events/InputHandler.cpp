#include "events/InputHandler.hpp"
#include "Game.hpp"
#include "states/StateMachine.hpp"
#include <iostream>

using namespace std;

Events::InputHandler::InputHandler(States::StateMachine& stateMachine)
	: m_stateMachine(stateMachine), m_totalTime(0)
{
}

void Events::InputHandler::Update(float dt)
{
	m_totalTime += dt;
}


//------------------------------------------------------------------------------
// KEYBOARD EVENTS

void Events::InputHandler::TextEntered(char character)
{
	m_stateMachine.TextEntered(character);
}

void Events::InputHandler::KeyPressed(sf::Event::KeyEvent& key)
{
	m_stateMachine.KeyPressed(key);
	m_keyLastPressed.erase(key.code);
	m_keyLastPressed.emplace(key.code, m_totalTime);
}

void Events::InputHandler::KeyReleased(sf::Event::KeyEvent& key)
{
	if (m_keyLastPressed.find(key.code) != m_keyLastPressed.end())
	{
		float time = m_totalTime - m_keyLastPressed[key.code];
		m_keyLastPressed.erase(key.code);
		m_stateMachine.KeyReleased(key, time);
	}
}


//------------------------------------------------------------------------------
// MOUSE EVENTS

void Events::InputHandler::MouseWheelMoved(sf::Event::MouseWheelEvent& wheel)
{
	m_stateMachine.MouseWheelMoved(wheel);
}

void Events::InputHandler::MouseButtonPressed(sf::Event::MouseButtonEvent& button)
{
	m_stateMachine.MouseButtonPressed(button);
	m_mouseBtnLastPressed.erase(button.button);
	m_mouseBtnLastPressed.emplace(button.button, m_totalTime);
}

void Events::InputHandler::MouseButtonReleased(sf::Event::MouseButtonEvent& button)
{
	if (m_mouseBtnLastPressed.find(button.button) != m_mouseBtnLastPressed.end())
	{
		float time = m_totalTime - m_mouseBtnLastPressed[button.button];
		m_mouseBtnLastPressed.erase(button.button);
		m_stateMachine.MouseButtonReleased(button, time);
	}
}

void Events::InputHandler::MouseMoved(sf::Event::MouseMoveEvent& move)
{
	m_stateMachine.MouseMoved(move.x - m_mousePos[0], move.y - m_mousePos[1]);

	// Update mouse position
	m_mousePos[0] = move.x;
	m_mousePos[1] = move.y;
}