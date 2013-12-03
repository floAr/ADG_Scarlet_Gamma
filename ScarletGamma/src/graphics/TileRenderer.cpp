#include "graphics/TileRenderer.hpp"
#include "core/World.hpp"
#include "core/Map.hpp"
#include "core/ObjectList.hpp"
#include "core/Object.hpp"
#include "sfutils/View.hpp"
#include "Game.hpp"
#include <cmath>
#include <iostream>

void Graphics::TileRenderer::Render(sf::RenderWindow& window, Core::Map& map)
{
	// Get map-coordinate float rect from the current camera for visibility tests
	sf::FloatRect viewRect = sfUtils::View::GetViewRect(window.getView());
	
	// Ceil / floor to find the top-left and bottom-right tile that is currently visible
	int tileSize = 64;
	int left = (int)floor(viewRect.left / tileSize);
	int top = (int)floor(viewRect.top / tileSize);
	int right = (int)ceil((viewRect.left + viewRect.width) / tileSize);
	int bottom = (int)ceil((viewRect.top + viewRect.height) / tileSize);

	for (int y = top; y <= bottom; y++)
	{
		for (int x = left; x <= right; x++)
		{
			Core::ObjectList& objList = map.GetObjectsAt(x, y);
			
			// Draw objects bottom to top
			for (int layer = 0; layer < objList.Size(); layer++)
			{
				// Get the object ID
				Core::ObjectID objID = objList[layer];
				
				// try to get the object instance from the world
				Core::Object* obj = g_Game->GetWorld()->GetObject(objID);
				assert(obj);

				// Render visible objects
				if (obj->HasProperty("Sprite"))
				{
					// Load texture
					// TODO: content manager! we don't wanna load this in every - fucking - frame!
					sf::Texture tex;
					tex.loadFromFile(obj->GetProperty("Sprite").Value());

					// Draw the tile
					sf::Sprite drawSprite(tex);
					drawSprite.setPosition(obj->GetPosition() * float(tileSize));
					drawSprite.setScale(float(tileSize)/tex.getSize().x, float(tileSize)/tex.getSize().y);
					window.draw(drawSprite);
				}
			}
		}
	}
}