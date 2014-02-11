#pragma once

#include "Prerequisites.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include "utils/Content.hpp"

namespace Graphics
{
	class AtlasSprite:public sf::Sprite
	{
	public :
		sf::Vector2f TC0()
		{
			return sf::Vector2f(m_TCbegin);
		}

		sf::Vector2f TC1()
		{
			return sf::Vector2f(m_TCbegin.x,m_TCend.y);
		}

		sf::Vector2f TC2()
		{
			return sf::Vector2f(m_TCend);
		}

		sf::Vector2f TC3()
		{
			return sf::Vector2f(m_TCend.x, m_TCbegin.y);
		}

		void SetTC(sf::Vector2f tcBegin,sf::Vector2f tcEnd)
		{
			m_TCbegin=tcBegin;
			m_TCend=tcEnd;
		}

	private:
		sf::Vector2f m_TCbegin;
		sf::Vector2f m_TCend;
	};

	/// \brief Renders the currently visible section of a given map.
	class SpriteAtlasBatcher:public sf::Drawable
	{
	public:
		static SpriteAtlasBatcher* Instance();
        AtlasSprite& AddOrGetAtlasSprite(const std::string& name);
		void Begin();
		void Enque(AtlasSprite as);
		void End();
	protected:
		SpriteAtlasBatcher();
	private:
		bool m_hasBegun;
		float m_yOffset;
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		sf::VertexArray m_drawArray;
		sf::Vector2f m_atlasBounds;
		static SpriteAtlasBatcher *m_instance;
		sf::RenderTexture m_atlasTexture;
		std::unordered_map<std::string,AtlasSprite> m_atlas;
	};
}