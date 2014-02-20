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
    enum MouseCursor
    {
        MC_DEFAULT = 0,
        MC_WALK,
        MC_ATTACK,
        MC_EXAMINE
    };

	/// \brief Default constructor that tames wild pointers
	Game();

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

	/// \brief Get the game's render window.
	/// \return Reference to the render window.
	sf::RenderWindow& GetWindow()
	{
		return m_window;
	}

	/// \brief Tell the game about the PlayerState / MasterState
	/// Is called from the respective state's constructor.
	void SetCommonState(States::CommonState* state)
	{
		m_commonState = state;
	}

	/// \brief Get the game's PlayerState / MasterState.
	/// Consult Messenger::IsServer() to find out which one it is.
	States::CommonState* GetCommonState()
	{
		return m_commonState;
	}

	void AppendToChatLog( const Network::ChatMsg& _message )	{ m_chatMessages.push_back(_message); ++m_numNewChatMessages; }
	bool HasLoggedNewChatMessages() const						{ return m_numNewChatMessages > 0; }
	const Network::ChatMsg& GetNextUntreatedChatMessage()		{ return m_chatMessages[m_chatMessages.size()-(m_numNewChatMessages--)]; }

	void UpdateMouseCursor();
	void SetMouseCursor(MouseCursor cursor);

private:
	Events::EventHandler* m_eventHandler;
	States::StateMachine* m_stateMachine;
	Core::World* m_world;
	sf::RenderWindow m_window;
	States::CommonState* m_commonState;
	sf::Sprite m_cursorSprite;

	/// \brief Collect messages independent from game state.
	/// \details This allows to commit the message history to a new player
	///		and to keep them over state changes. Sometimes a menu state or
	///		something like that is on top of the state stack so the state
	///		containing the chat window cannot be reached. In this case new
	///		lines are buffered until that state is updated the next time.
	std::vector<Network::ChatMsg> m_chatMessages;
	int m_numNewChatMessages;	///< Number of untreated incoming chat messages.

	float m_resourceSyncTime;

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