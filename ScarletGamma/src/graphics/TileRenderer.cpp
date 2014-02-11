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
#include "SpriteAtlasBatcher.hpp"


void Graphics::TileRenderer::Render(sf::RenderWindow& window, Core::Map& map, std::function<float(Core::Map&,sf::Vector2i&)> _tileVisible,const bool* hiddenLayers)
{
	// Get map-coordinate float rect from the current camera for visibility tests
	sf::FloatRect viewRect = sfUtils::View::GetViewRect(window.getView());
	
	// Ceil / floor to find the top-left and bottom-right tile that is currently visible
	int left   = (int) std::max(floor(viewRect.left / TILESIZE), (float) map.Left());
	int top    = (int) std::max(floor(viewRect.top / TILESIZE), (float) map.Top());
	int right  = (int) std::min(ceil((viewRect.left + viewRect.width) / TILESIZE), (float) map.Right());
	int bottom = (int) std::min(ceil((viewRect.top + viewRect.height) / TILESIZE), (float) map.Bottom());

	Graphics::SpriteAtlasBatcher::Instance()->Begin();

	// Search layer-wise
	for (int layer = 0; layer <= map.GetMaxLayer(); layer++ )
	{
		if(hiddenLayers!=nullptr){
			if(hiddenLayers[layer])
				continue;
		}

		for (int y = top; y <= bottom; ++y)
		{
			for (int x = left; x <= right; ++x)
			{
				float visibility = _tileVisible( map, sf::Vector2i(x, y) );
				if( visibility > 0.01f )
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
						if( atoi(obj->GetProperty(STR_PROP_LAYER).Value().c_str()) != layer )
							continue;

						// Render visible objects
						// TODO: property for visibility
						if (obj->HasProperty(STR_PROP_SPRITE))
						{
							// Draw the tile
							auto& drawSprite = Graphics::SpriteAtlasBatcher::Instance()->AddOrGetAtlasSprite(obj->GetProperty(STR_PROP_SPRITE).Value());
							drawSprite.setPosition(obj->GetPosition() * float(TILESIZE));
							sf::Color color = obj->GetColor();
							color.a = uint8_t(color.a * visibility);
							drawSprite.setColor(color);
							Graphics::SpriteAtlasBatcher::Instance()->Enque(drawSprite);
						}
					}
				}
			}
		}
	} // layer

	Graphics::SpriteAtlasBatcher::Instance()->End();
	window.draw(*Graphics::SpriteAtlasBatcher::Instance());
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

void Graphics::TileRenderer::RenderSelection( sf::RenderWindow& _window, const Core::ObjectList& _selection )
{
	const sf::Texture& tex = Content::Instance()->LoadTexture("media/selected.png");
	sf::Sprite drawSprite(tex);
	drawSprite.setColor(sf::Color(255, 255, 1, 255));
	for( int i=0; i<_selection.Size(); ++i )
	{
		Core::Object* obj = g_Game->GetWorld()->GetObject( _selection[i] );
		drawSprite.setPosition(obj->GetPosition() * float(TILESIZE));
		drawSprite.setScale(float(TILESIZE)/tex.getSize().x, float(TILESIZE)/tex.getSize().y);
		_window.draw(drawSprite);
	}
}


void Graphics::TileRenderer::RenderRect(sf::RenderWindow& _window, const sf::Vector2i& _min, const sf::Vector2i& _max)
{
	const sf::Texture& tex = Content::Instance()->LoadTexture("media/selected.png");
	sf::Sprite drawSprite(tex);
	drawSprite.setColor(sf::Color(1, 255, 255, 255));
	drawSprite.setPosition(sf::Vector2f(_min) * float(TILESIZE));
	drawSprite.setScale(float((_max.x-_min.x) * TILESIZE)/tex.getSize().x, float((_max.y-_min.y) * TILESIZE)/tex.getSize().y);
	_window.draw(drawSprite);
}
