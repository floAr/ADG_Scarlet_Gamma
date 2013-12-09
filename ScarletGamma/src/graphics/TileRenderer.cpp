#include "graphics/TileRenderer.hpp"
#include "core/World.hpp"
#include "core/Map.hpp"
#include "core/ObjectList.hpp"
#include "core/Object.hpp"
#include "sfutils/View.hpp"
#include "Game.hpp"
#include "Constants.hpp"
#include <cmath>
#include <iostream>
#include <unordered_map>

void Graphics::TileRenderer::Render(sf::RenderWindow& window, Core::Map& map)
{
	// Get map-coordinate float rect from the current camera for visibility tests
	sf::FloatRect viewRect = sfUtils::View::GetViewRect(window.getView());
	
	// Ceil / floor to find the top-left and bottom-right tile that is currently visible
	int left = (int)floor(viewRect.left / TILESIZE);
	int top = (int)floor(viewRect.top / TILESIZE);
	int right = (int)ceil((viewRect.left + viewRect.width) / TILESIZE);
	int bottom = (int)ceil((viewRect.top + viewRect.height) / TILESIZE);


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

					// Very quick and dirty custom resource manager
					static std::unordered_map<std::string, sf::Texture> textures;
					if (textures.find(obj->GetProperty("Sprite").Value()) == textures.end())
					{
						sf::Texture tex;
						tex.loadFromFile(obj->GetProperty("Sprite").Value());
						textures.emplace(obj->GetProperty("Sprite").Value(), tex);
					}
					sf::Texture& tex = textures[obj->GetProperty("Sprite").Value()];

					// Draw the tile
					sf::Sprite drawSprite(tex);
					drawSprite.setPosition(obj->GetPosition() * float(TILESIZE));
					drawSprite.setScale(float(TILESIZE)/tex.getSize().x, float(TILESIZE)/tex.getSize().y);
					drawSprite.setColor(obj->GetColor());
					window.draw(drawSprite);
				}
			}
		}
	}
}