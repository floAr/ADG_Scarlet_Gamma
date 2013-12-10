#pragma once

#include "Prerequisites.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

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

		/// \brief Draw a path overlay for the current objects path.
		///	\param [in] An array of tile-positions where the overlay should appear.
		static void RenderPath(sf::RenderWindow& window, const std::vector<sf::Vector2i>& _path);
	};
}