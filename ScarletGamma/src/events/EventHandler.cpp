#include "events/EventHandler.hpp"
#include "events/InputHandler.hpp"
#include "Game.hpp"

Events::EventHandler::EventHandler(sf::Window& window)
	: m_window(window)
{
	m_inputHandler = new InputHandler();
}

Events::EventHandler::~EventHandler()
{
	delete m_inputHandler;
}

void Events::EventHandler::Update()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		switch (event.type)
		{

		//----------------------------------------------------------------
		// WINDOW EVENTS
		case sf::Event::Closed:
			m_window.close();
			break;
		case sf::Event::Resized:
			break;
		case sf::Event::LostFocus:
			break;
		case sf::Event::GainedFocus:
			break;

		//----------------------------------------------------------------
		// KEYBOARD EVENTS
		case sf::Event::TextEntered:
			break;
		case sf::Event::KeyPressed:
			break;
		case sf::Event::KeyReleased:
			break;

		//----------------------------------------------------------------
		// MOUSE EVENTS
		case sf::Event::MouseWheelMoved:
			break;
		case sf::Event::MouseButtonPressed:
			break;
		case sf::Event::MouseButtonReleased:
			break;
		case sf::Event::MouseMoved:
			break;
		case sf::Event::MouseEntered:
			break;
		case sf::Event::MouseLeft:
			break;

		//----------------------------------------------------------------
		// JOYSTICK / GAMEPAD EVENTS
		case sf::Event::JoystickButtonPressed:
			break;
		case sf::Event::JoystickButtonReleased:
			break;
		case sf::Event::JoystickMoved:
			break;
		case sf::Event::JoystickConnected:
			break;
		case sf::Event::JoystickDisconnected:
			break;

		default:
			break;
		}
	}
}