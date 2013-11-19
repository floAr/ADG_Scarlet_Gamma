#include "GraphicsManager.hpp"

Graphics::GraphicsManager::GraphicsManager()
{
	m_window.create(sf::VideoMode(800, 600), "My window");
}