#include "Game.hpp"
#include "events/EventHandler.hpp"
#include "states/StateMachine.hpp"
#include "states/GameState.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/World.hpp"
#include "core/Map.hpp"
#include "utils/Random.hpp"
#include "sfutils/View.hpp"
#include "states/CommonState.hpp"
#include <math.h>

Utils::Random* Game::RANDOM = new Utils::Random((uint32_t) time(0));

Game::Game() : m_stateMachine(0), m_world(0), m_eventHandler(0), m_numNewChatMessages(0)
{
	m_cursorSprite.setTexture(Content::Instance()->LoadTexture("media/cursors.png"));
	SetMouseCursor(MC_DEFAULT);
}

void Game::Init()
{
	// Create a state machine
	m_stateMachine = new States::StateMachine();

	// Set up SFML window
	m_window.create(sf::VideoMode(1024, 768), "Scarlet Gamma");
	m_window.setVerticalSyncEnabled(true);
	m_window.setMouseCursorVisible(false);
	m_window.setKeyRepeatEnabled(true); // PLEASE LEAVE THIS ON, because the GUI will suck otherwise.
										// If this has a bad effect on gameplay, we'll need to find a workaround.
										//   ~Daerst

	// Create an event handler using the window
	m_eventHandler = new Events::EventHandler(m_window, *m_stateMachine);

	// Create an empty game world
	m_world = new Core::World();

	// Push states. Note that the last state is current!
	m_stateMachine->PushGameState(States::GST_MAIN_MENU);
	m_stateMachine->PushGameState(States::GST_INTRO);

#ifdef _FPS
	m_dFps = 60;
	m_dFpsTime = 0;
	m_dFpsCounter = 0;
	m_dFpsFont.loadFromFile("media/arial.ttf");
#endif
}

void Game::Run()
{
	sf::Clock clock;
	while (m_window.isOpen() && m_stateMachine->HasStates())
	{
		// Get elapsed time and add it to total running time
		float time = clock.restart().asSeconds();

		// Let the EventHandler handle all window events
		m_eventHandler->Update(time);

		// Update and draw the GameState
		m_stateMachine->Update(time);
		m_stateMachine->Draw(m_window);

#ifdef _FPS
		m_dFpsTime += time;
		m_dFpsCounter++;
		if (m_dFpsTime >= 1.f)
		{
			m_dFps = m_dFpsCounter;
			m_dFpsTime = 0;
			m_dFpsCounter = 0;
		}
		sf::View fpsViewBackup = sfUtils::View::SetDefault(&m_window);
		sf::Text t(std::to_string(m_dFps), m_dFpsFont, 12);
		if (m_dFps < 30)
			t.setColor(sf::Color::Red);
		t.setPosition(5, 5);
		m_window.draw(t);
		m_window.setView(fpsViewBackup);
#endif
		// Draw cursor with default view
		sf::View mouseViewBackup = sfUtils::View::SetDefault(&m_window);
		m_window.draw(m_cursorSprite);
		m_window.setView(mouseViewBackup);

		// Swap buffers
		m_window.display();
	}
}

void Game::UpdateMouseCursor()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
	m_cursorSprite.setPosition((float) mousePos.x, (float) mousePos.y);
}

void Game::SetMouseCursor(MouseCursor cursor)
{
	static const uint8_t imageSize  = 4;     // 4 cursors side by side
	static const uint8_t cursorSize = 36;   // Each cursor is 36x36 px

	uint8_t x = cursor % imageSize;
	uint8_t y = cursor / imageSize;

	sf::IntRect textureRect(x * cursorSize, y * cursorSize, cursorSize, cursorSize);
	m_cursorSprite.setTextureRect(textureRect);
}

void Game::CleanUp()
{
	// Delete in reverse order of construction
	delete m_eventHandler;
	delete m_stateMachine;
	delete m_world;
}
