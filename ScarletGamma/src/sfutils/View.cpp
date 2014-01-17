#include "sfutils/View.hpp"

sf::FloatRect sfUtils::View::GetViewRect(const sf::View& view)
{
	const sf::Vector2f size = view.getSize();
	const sf::Vector2f center = view.getCenter();
	return sf::FloatRect(center.x - size.x / 2.f, center.y - size.y / 2.f, size.x, size.y);
}

sf::View sfUtils::View::SetDefault(sf::RenderWindow* _window)
{
	sf::View backup = _window->getView();
	sf::View guiView;
	guiView.setSize((float)_window->getSize().x, (float)_window->getSize().y);
	guiView.setCenter(guiView.getSize() * 0.5f);
	_window->setView(guiView);
	return backup;
}
