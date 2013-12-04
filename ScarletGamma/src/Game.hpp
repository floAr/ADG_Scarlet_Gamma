#pragma once

#include "Prerequisites.hpp"
#include <assert.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

/// \brief Master class containing the game loop and pointers to subsystems.
///
/// The main() creates and deletes a global Game into the g_Game variable.
class Game
{
public:

	/// \brief Default constructor that tames wild pointers
	Game() : m_stateMachine(0), m_world(0), m_eventHandler(0) {}

	/// TODO:: Document
	void Init();

	/// \brief Starts the GameLoop.
	/// \details Runs until the StateMachine tells it to stop.
	void Run();

	/// TODO: Document
	void CleanUp();

	/// \brief Get the game's state machine.
	/// \return Pointer to StateMachine object
	States::StateMachine* GetStateMachine()
	{
		assert(m_stateMachine);
		return m_stateMachine;
	}

	/// \brief Get the game's world.
	/// \return Pointer to World object
	Core::World* GetWorld()
	{
		assert(m_world);
		return m_world;
	}

private:  
	Events::EventHandler* m_eventHandler;
	States::StateMachine* m_stateMachine;
	Core::World* m_world;
	sf::RenderWindow m_window;
};

/// \brief Global pointer because everyone needs to know the game anyway...
/// \detail Externally defined in main.cpp
extern Game* g_Game;