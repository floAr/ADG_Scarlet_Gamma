#include "states/PlayerState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"

void States::PlayerState::Update(float dt)
{
}

void States::PlayerState::Draw(sf::RenderWindow& win)
{
	// Draw some color to the background
	static sf::Color c(20, 26, 36);
	win.clear(c);

	// Draw tile map
	static Core::Map map(0, "RenderTest", 64, 64, g_Game->GetWorld());
	map.Add(0, 1, 2, 0);
	Graphics::TileRenderer::Render(win, map);
}