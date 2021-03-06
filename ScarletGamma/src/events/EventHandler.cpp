#include "events/EventHandler.hpp"
#include "events/InputHandler.hpp"
#include "states/StateMachine.hpp"
#include "Game.hpp"
#include <windows.h>

Events::EventHandler::EventHandler(sf::Window& window, States::StateMachine& stateMachine)
	: m_window(window), m_fullscreen(false), m_windowStyle(0)
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

	// Get GameState - we don't even wanna use it, just see if someone pushed a new one
	void* gameState = g_Game->GetStateMachine()->GetGameState();

	// Process events
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// Forward events to GUI
		bool guiHandled = g_Game->GetStateMachine()->GuiHandleEvent(event);

		switch (event.type)
		{

		//----------------------------------------------------------------
		// WINDOW EVENTS

		case sf::Event::Closed:
			m_window.close();
			break;

		case sf::Event::Resized: {
			sf::View V = dynamic_cast<sf::RenderWindow&>(m_window).getView();
			V.setSize( (float) event.size.width, (float) event.size.height );
			dynamic_cast<sf::RenderWindow&>(m_window).setView(V);
			g_Game->GetStateMachine()->Resize( sf::Vector2f((float) event.size.width,
				(float)event.size.height) );
			} break;

		case sf::Event::LostFocus:
			break;

		case sf::Event::GainedFocus:
			break;

		//----------------------------------------------------------------
		// KEYBOARD EVENTS

		case sf::Event::TextEntered:
			// cast to ASCII
			if (event.text.unicode < 256)
				m_inputHandler->TextEntered(static_cast<char>(event.text.unicode), guiHandled);
			break;

		case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::F11 || (event.key.alt && event.key.code == sf::Keyboard::Return))
            {
                if (!m_fullscreen)
                {
                    // Save state
                    m_windowStyle = GetWindowLong(m_window.getSystemHandle(), GWL_STYLE);
                    m_windowSize = m_window.getSize();
                    m_windowPos = m_window.getPosition();

                    // Set to fullscreen
                    RECT desktop;
                    GetWindowRect(GetDesktopWindow(), &desktop);
                    SetWindowLong(m_window.getSystemHandle(), GWL_STYLE, 0);
                    ShowWindow(m_window.getSystemHandle(), SW_SHOW);
                    m_window.setPosition(sf::Vector2i(0, 0));
                    m_window.setSize(sf::Vector2u(desktop.right, desktop.bottom));
                }
                else
                {
                    // Restore windowed mode
                    SetWindowLong(m_window.getSystemHandle(), GWL_STYLE, m_windowStyle);
                    m_window.setSize(m_windowSize);
                    m_window.setPosition(m_windowPos);
                }

                // Toggle fullscreen variable
                m_fullscreen = !m_fullscreen;
            }
			m_inputHandler->KeyPressed(event.key, guiHandled);
			break;

		case sf::Event::KeyReleased:
			m_inputHandler->KeyReleased(event.key, guiHandled);
			break;

		//----------------------------------------------------------------
		// MOUSE EVENTS

		case sf::Event::MouseWheelMoved:
			m_inputHandler->MouseWheelMoved(event.mouseWheel, guiHandled);
			break;

		case sf::Event::MouseButtonPressed:
			// SFML bugfix: Focus window
			SetFocus(m_window.getSystemHandle());
			m_inputHandler->MouseButtonPressed(event.mouseButton, guiHandled);
			break;

		case sf::Event::MouseButtonReleased:
			m_inputHandler->MouseButtonReleased(event.mouseButton, guiHandled);
			break;

		case sf::Event::MouseMoved:
			g_Game->UpdateMouseCursor(); // Notify the game
			m_inputHandler->MouseMoved(event.mouseMove, guiHandled);
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

		// If anyone pushed a GameState, neglect the remaining events
		if (gameState != g_Game->GetStateMachine()->GetGameState())
		{
			while (m_window.pollEvent(event));
			return;
		}
	}
}