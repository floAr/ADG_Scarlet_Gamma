#include "states/PlayerState.hpp"
#include "Game.hpp"
#include "graphics/TileRenderer.hpp"
#include "core/Map.hpp"
#include "core/World.hpp"

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

	// Create some random objects
	Core::ObjectID id;
	for (int i=0; i<10; ++i)
	{
		id = g_Game->GetWorld()->NewObject("media/test.png");
		map.Add(id, i, i, 0);
	}

	// Render
	Graphics::TileRenderer::Render(win, map);
}