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

void Events::InputHandler::TextEntered(char character)
{
	m_stateMachine.TextEntered(character);
}

void Events::InputHandler::KeyPressed(sf::Event::KeyEvent key)
{
	m_stateMachine.KeyPressed(key);
	m_keyLastPressed.erase(key.code);
	m_keyLastPressed.emplace(key.code, m_totalTime);

}

void Events::InputHandler::KeyReleased(sf::Event::KeyEvent key)
{
	if (m_keyLastPressed.find(key.code) != m_keyLastPressed.end())
	{
		float time = m_totalTime - m_keyLastPressed[key.code];
		m_keyLastPressed.erase(key.code);
		m_stateMachine.KeyReleased(key, time);
	}
}