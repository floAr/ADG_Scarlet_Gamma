#pragma once
#include "Prerequisites.hpp"
#include <SFML/Window.hpp>

namespace Events
{
	class EventHandler
	{
	public:
		EventHandler(sf::Window& window);
		~EventHandler();
		void Update();
	private:
		sf::Window& m_window;
		InputHandler* m_inputHandler;
	};
}