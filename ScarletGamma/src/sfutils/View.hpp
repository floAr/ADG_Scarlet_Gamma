#pragma once

#include <SFML/Graphics.hpp>

namespace sfUtils
{
	/// \brief Utility class providing static functions to help with the

	class View
	{
	private:
		/// \brief Prohibit construction.
		View() {}

	public:
		/// \brief Gets the view rectangle from a view in map coordinates
		static sf::FloatRect GetViewRect(const sf::View& view);

		/// \brief Sets a view which has exactly window size and (0,0) at the
		///		top left corner.
		///	\return The view which was set before.
		static sf::View SetDefault(sf::RenderWindow* _window);
	};
}