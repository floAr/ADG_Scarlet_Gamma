#pragma once
#ifdef _DEBUG
	#define _FPS
#endif

#include "Prerequisites.hpp"
#include <assert.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "network/ChatMessages.hpp"
#include <ctime>

/// \brief Master class containing the game loop and pointers to subsystems.
///
/// The main() creates and deletes a global Game into the g_Game variable.
class Game
{
public:

	/// \brief Default constructor that tames wild pointers
	Game() : m_stateMachine(0), m_world(0), m_eventHandler(0), m_numNewChatMessages(0) {}

    static Utils::Random* RANDOM;

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

	sf::RenderWindow& GetWindow()
	{
		return m_window;
	}

	void AppendToChatLog( const Network::ChatMsg& _message )	{ m_chatMessages.push_back(_message); ++m_numNewChatMessages; }
	bool HasLoggedNewChatMessages() const						{ return m_numNewChatMessages > 0; }
	const Network::ChatMsg& GetNextUntreatedChatMessage()		{ return m_chatMessages[m_chatMessages.size()-(m_numNewChatMessages--)]; }

private:  
	Events::EventHandler* m_eventHandler;
	States::StateMachine* m_stateMachine;
	Core::World* m_world;
	sf::RenderWindow m_window;

	/// \brief Collect messages independent from game state.
	/// \details This allows to commit the message history to a new player
	///		and to keep them over state changes. Sometimes a menu state or
	///		something like that is on top of the state stack so the state
	///		containing the chat window cannot be reached. In this case new
	///		lines are buffered until that state is updated the next time.
	std::vector<Network::ChatMsg> m_chatMessages;
	int m_numNewChatMessages;	///< Number of untreated incoming chat messages.

#ifdef _FPS
	float    m_dFpsTime;
	int      m_dFpsCounter;
	int      m_dFps;
	sf::Font m_dFpsFont;
#endif
};

/// \brief Global pointer because everyone needs to know the game anyway...
/// \detail Externally defined in main.cpp
extern Game* g_Game;