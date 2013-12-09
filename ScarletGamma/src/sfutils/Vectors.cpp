#include "Vectors.hpp"

namespace sfUtils {

std::string to_string( const sf::Vector2f& _v )
{
	return std::to_string(_v.x) + ':' + std::to_string(_v.y);
}

sf::Vector2f sfUtils::to_vector( const std::string& _str )
{
	sf::Vector2f result;
	sscanf( _str.c_str(), "%f:%f", &result.x, &result.y);
	return result;
}


} // namespace sfUtils