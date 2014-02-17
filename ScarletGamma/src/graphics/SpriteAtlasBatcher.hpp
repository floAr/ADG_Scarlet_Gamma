#pragma once

#include "Prerequisites.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include "utils/Content.hpp"

namespace Graphics
{
	// TODO: DOCUMENT THIS CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// WHAT THE HELL IS TC ????
	// USE BETTER NAMES + DESCRIBING DOC
	class AtlasSprite: public sf::Sprite
	{
	public :
		/// \brief TC0-TC3 return the corresponding texture coordinates in counter clockwise order
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

		void SetTC(sf::Vector2f _tcBegin,sf::Vector2f _tcEnd)
		{
			m_TCbegin = _tcBegin;
			m_TCend = _tcEnd;
		}

		bool IsDefault()
		{
			return m_isDefault;
		}
		void SetDefaultState(bool _isDefault)
		{
			m_isDefault=_isDefault;
		}

	private:
		sf::Vector2f m_TCbegin;	///< Top-Left TextureCoord
		sf::Vector2f m_TCend;	///< Bottom-Right TextureCoord
		bool m_isDefault;		///< Indicates if this used the dummy sprite
	};

	/// \brief Renders the currently visible section of a given map.
	class SpriteAtlasBatcher: public sf::Drawable
	{
	public:
		static SpriteAtlasBatcher* Instance();
        AtlasSprite& AddOrGetAtlasSprite(const std::string& _name);
		void Begin();
		void Enque(AtlasSprite as);
		void End();
	protected:
		SpriteAtlasBatcher();
	private:
		bool m_textureDirty;
		bool m_hasBegun;
		float m_yOffset;
		virtual void draw(sf::RenderTarget& _target, sf::RenderStates _states) const;
		sf::VertexArray m_drawArray;
		sf::Vector2f m_atlasBounds;
		sf::RenderTexture m_atlasTexture;
		std::unordered_map<std::string,AtlasSprite> m_atlas;
	};
}