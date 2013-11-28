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
	};
}