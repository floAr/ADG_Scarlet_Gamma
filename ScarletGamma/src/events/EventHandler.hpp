#pragma once
#include "Prerequisites.hpp"
#include <SFML/Window.hpp>

namespace Events
{
	class EventHandler
	{
	public:
		EventHandler(sf::Window& window, States::StateMachine& stateMachine);
		~EventHandler();
		void Update(float dt);
	private:
		sf::Window& m_window;
		InputHandler* m_inputHandler;
	};
}