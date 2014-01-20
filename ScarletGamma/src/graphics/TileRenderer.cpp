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

	// Search layer-wise
	for (int layer = 0; layer <= map.GetMaxLayer(); layer++ )
	{
		for (int y = top; y <= bottom; y++)
		{
			for (int x = left; x <= right; x++)
			{
				Core::ObjectList& objList = map.GetObjectsAt(x, y);
			
				// Draw objects bottom to top
				for (int i = 0; i < objList.Size(); i++)
				{
					// Get the object ID
					Core::ObjectID objID = objList[i];
				
					// try to get the object instance from the world
					Core::Object* obj = g_Game->GetWorld()->GetObject(objID);
					assert(obj);

					// Skip objects from the wrong layer
					if( atoi(obj->GetProperty(Core::Object::PROP_LAYER).Value().c_str()) != layer )
						continue;

					// Render visible objects TODO visib-prop
					if (obj->HasProperty(Core::Object::PROP_SPRITE))
					{
						// Load texture

						// Draw the tile
						const sf::Texture& tex = Content::Instance()->LoadTexture(obj->GetProperty(Core::Object::PROP_SPRITE).Value());
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
}

void Graphics::TileRenderer::RenderPath( sf::RenderWindow& window, const std::vector<sf::Vector2i>& _path )
{
	const sf::Texture& tex = Content::Instance()->LoadTexture("media/way_point.png");
	sf::Sprite drawSprite(tex);
	drawSprite.setColor(sf::Color(255, 144, 1, 130));
	for( size_t i=0; i<_path.size(); ++i )
	{
		drawSprite.setPosition(sf::Vector2f(_path[i]) * float(TILESIZE));
		drawSprite.setScale(float(TILESIZE)/tex.getSize().x, float(TILESIZE)/tex.getSize().y);
		window.draw(drawSprite);
	}
}

void Graphics::TileRenderer::RenderSelection( sf::RenderWindow& window, const Core::ObjectList& _selection )
{
	const sf::Texture& tex = Content::Instance()->LoadTexture("media/selected.png");
	sf::Sprite drawSprite(tex);
	drawSprite.setColor(sf::Color(255, 255, 1, 255));
	for( int i=0; i<_selection.Size(); ++i )
	{
		Core::Object* obj = g_Game->GetWorld()->GetObject( _selection[i] );
		drawSprite.setPosition(obj->GetPosition() * float(TILESIZE));
		drawSprite.setScale(float(TILESIZE)/tex.getSize().x, float(TILESIZE)/tex.getSize().y);
		window.draw(drawSprite);
	}
}
