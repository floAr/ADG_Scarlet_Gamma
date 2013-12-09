#include "SFML/System/Vector2.hpp"
#include <string>

namespace sfUtils {

	/// \brief Compute the length of a vector with two elements
	template<typename T> float Length( sf::Vector2<T> _v )	{
		return (float)sqrt(_v.x*_v.x + _v.y*_v.y);
	}

	/// \brief Compute the quadratic length of a vector with two elements
	template<typename T> T LengthSq( sf::Vector2<T> _v )	{
		return _v.x*_v.x + _v.y*_v.y;
	}

	/// \brief Convert a vector to "%f:%f".
	std::string to_string(const sf::Vector2f& _v);

	/// \brief Convert a string "%f:%f" into a two component vector.
	sf::Vector2f to_vector(const std::string& _str);

	inline sf::Vector2i Round(const sf::Vector2f& _v)	{
		return sf::Vector2i(static_cast<int>(floor(_v.x+0.5f)), static_cast<int>(floor(_v.y+0.5f)));
	}
} // namespace sfUtils