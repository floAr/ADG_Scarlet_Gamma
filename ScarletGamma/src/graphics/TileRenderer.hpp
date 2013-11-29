#pragma once

#include "Prerequisites.hpp"
#include <SFML/Graphics.hpp>

namespace Graphics
{
	/// \brief Renders the currently visible section of a given map.
	class TileRenderer
	{
	public:
		/// \brief Renders the currently visible section of a given map to the specified window.
		/// \param [in] window	SFML Window to be rendered to
		/// \param [in] map		Map to be rendered
		static void Render(sf::RenderWindow& window, Core::Map& map);
	};
}