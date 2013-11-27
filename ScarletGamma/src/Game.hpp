#pragma once

#include "Prerequisites.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

/// \brief Master class containing the game loop and pointers to subsystems.
///
/// The main() creates and deletes a global Game into the g_Game variable.
class Game
{
public:
	/// \todo Document
	void Init();

	/// \brief Starts the GameLoop.
	/// \details Runs until the StateMachine tells it to stop.
	void Run();

	/// \todo Document
	void CleanUp();

private:
	States::StateMachine*		m_stateMachine;
	Graphics::TileRenderer*		m_tileRenderer;
	sf::RenderWindow			m_window;
};

/// \brief Global pointer because everyone needs to know the game anyway...
/// \detail Externally defined in main.cpp
extern Game* g_Game;