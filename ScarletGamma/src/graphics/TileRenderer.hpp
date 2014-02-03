#pragma once

#include "Prerequisites.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include "utils/Content.hpp"

namespace Graphics
{
	/// \brief Renders the currently visible section of a given map.
	class TileRenderer
	{
	public:

		/// \brief Renders the currently visible section of a given map to the specified window.
		/// \param [in] window	SFML Window to be rendered to
		/// \param [in] map		Map to be rendered
		/// \param [in] _tileVisible A black-box test to check if a player can
		///		see a certain tile. The function must take a tile position on
		///		a map and return its visibility as a percentage value in [0,1].
		/// \param [in] hiddenLayers Vector of bools to indicate which layers should be hidden. default is nullptr
		static void Render(sf::RenderWindow& window, Core::Map& map, std::function<float(Core::Map&,sf::Vector2i&)> _tileVisible,const bool* hiddenLayers=nullptr);


		/// \brief Draw a path overlay for the current objects path.
		///	\param [in] An array of tile-positions where the overlay should appear.
		static void RenderPath(sf::RenderWindow& window, const std::vector<sf::Vector2i>& _path);

		/// \brief Draw a focus selected objects.
		static void RenderSelection(sf::RenderWindow& _window, const Core::ObjectList& _selection);

		/// \brief Draw a rectangle for selection, brush, ...
		static void RenderRect(sf::RenderWindow& _window, const sf::Vector2i& _min, const sf::Vector2i& _max);
	};
}