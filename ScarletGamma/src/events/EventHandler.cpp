#include "events/EventHandler.hpp"
#include "events/InputHandler.hpp"
#include "states/StateMachine.hpp"
#include "Game.hpp"

Events::EventHandler::EventHandler(sf::Window& window, States::StateMachine& stateMachine)
	: m_window(window)
{
	m_inputHandler = new InputHandler(stateMachine);
}

Events::EventHandler::~EventHandler()
{
	delete m_inputHandler;
}

void Events::EventHandler::Update(float dt)
{
	// Update the InputHandler
	m_inputHandler->Update(dt);

	// Process events
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// Forward events to GUI
		if (g_Game->GetStateMachine()->GuiHandleEvent(event))
			continue;

		switch (event.type)
		{

		//----------------------------------------------------------------
		// WINDOW EVENTS

		case sf::Event::Closed:
			m_window.close();
			break;

		case sf::Event::Resized: {
			sf::View V = dynamic_cast<sf::RenderWindow&>(m_window).getView();
			V.setSize( event.size.width, event.size.height );
			dynamic_cast<sf::RenderWindow&>(m_window).setView(V);
			} break;

		case sf::Event::LostFocus:
			break;

		case sf::Event::GainedFocus:
			break;

		//----------------------------------------------------------------
		// KEYBOARD EVENTS

		case sf::Event::TextEntered:
			// cast to ASCII
			if (event.text.unicode < 128)
				m_inputHandler->TextEntered(static_cast<char>(event.text.unicode));
			break;

		case sf::Event::KeyPressed:
			m_inputHandler->KeyPressed(event.key);
			break;

		case sf::Event::KeyReleased:
			m_inputHandler->KeyReleased(event.key);
			break;

		//----------------------------------------------------------------
		// MOUSE EVENTS

		case sf::Event::MouseWheelMoved:
			m_inputHandler->MouseWheelMoved(event.mouseWheel);
			break;

		case sf::Event::MouseButtonPressed:
			m_inputHandler->MouseButtonPressed(event.mouseButton);
			break;

		case sf::Event::MouseButtonReleased:
			m_inputHandler->MouseButtonReleased(event.mouseButton);
			break;

		case sf::Event::MouseMoved:
			m_inputHandler->MouseMoved(event.mouseMove);
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
		}
	}
}