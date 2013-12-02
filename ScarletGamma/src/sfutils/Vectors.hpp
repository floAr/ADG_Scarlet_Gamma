#include "SFML/System/Vector2.hpp"

namespace sfUtils {

	/// \brief Compute the length of a vector with two elements
	template<typename T> float Length( sf::Vector2<T> _v )	{
		return (float)sqrt(_v.x*_v.x + _v.y*_v.y);
	}

	/// \brief Compute the quadratic length of a vector with two elements
	template<typename T> T LengthSq( sf::Vector2<T> _v )	{
		return _v.x*_v.x + _v.y*_v.y;
	}

} // namespace sfUtils