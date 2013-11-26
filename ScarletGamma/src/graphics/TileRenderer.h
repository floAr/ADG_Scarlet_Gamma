#pragma once

#include "Prerequisites.hpp"
#include <SFML/Graphics.hpp>

namespace Graphics
{
	class TileRenderer
	{
	public:
		TileRenderer(sf::RenderWindow& window);
		void Render(Core::Map& map);

	private:
		sf::RenderWindow& m_window;
	};
}