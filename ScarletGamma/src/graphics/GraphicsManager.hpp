#pragma once

#include <SFML/Graphics.hpp>

namespace Graphics
{
	/// \brief Keeps track of all necessary classes for displaying stuff on screen.
	/// \detail Don't confuse with the ResourceManager, that keeps track of the
	///		graphic files! This class uses SFML to create window and everything.
	class GraphicsManager
	{
	public:
		GraphicsManager();

	private:
		sf::RenderWindow m_window;
	};
}