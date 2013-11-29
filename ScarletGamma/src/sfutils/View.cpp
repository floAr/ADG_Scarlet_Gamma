#include "sfutils/View.h"

sf::FloatRect sfUtils::View::GetViewRect(const sf::View& view)
{
	const sf::Vector2f size = view.getSize();
	const sf::Vector2f center = view.getCenter();
	return sf::FloatRect(center.x - size.x / 2.f, center.y - size.y / 2.f, size.x, size.y);
}