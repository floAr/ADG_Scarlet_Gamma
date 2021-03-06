#include "graphics/SpriteAtlasBatcher.hpp"
#include "Constants.hpp"


using namespace Graphics;



SpriteAtlasBatcher* SpriteAtlasBatcher::Instance()
{
	static SpriteAtlasBatcher Instance;
	return &Instance;
}

SpriteAtlasBatcher::SpriteAtlasBatcher() : m_drawArray(sf::Quads), m_atlasBounds(0, 0), m_hasBegun(false), m_textureDirty(true), m_yOffset(0)
{
	m_atlasTexture.create(512, 512);
}


AtlasSprite& Graphics::SpriteAtlasBatcher::AddOrGetAtlasSprite( const std::string& _name )
{
	if(m_atlas.count(_name))
	{
		auto& as = m_atlas.at(_name);
		if(!as.IsDefault())
			return as;
	}

	AtlasSprite result;
	bool isDefault(false);
	const sf::Texture& tex = Content::Instance()->LoadTexture(_name,&isDefault);
	result.SetDefaultState(isDefault);
	if(!(m_atlasBounds.x+tex.getSize().x<m_atlasTexture.getSize().x))//object is nolonger matching on x axsis
	{
		m_atlasBounds.x = 0;
		m_atlasBounds.y += m_yOffset;
		m_yOffset = 0;
	}
	result.setTexture(tex, true);
	result.setPosition(m_atlasBounds);
	result.setScale(float(TILESIZE)/tex.getSize().x, float(TILESIZE)/tex.getSize().y);

	auto rBounds=result.getGlobalBounds();
	result.SetTC(m_atlasBounds, m_atlasBounds+sf::Vector2f(rBounds.width,rBounds.height));

	m_atlasTexture.draw(result);
	m_atlasBounds.x += rBounds.width;
	if(rBounds.height > m_yOffset)
		m_yOffset = (float) rBounds.height;

	m_textureDirty = true;

	m_atlas.emplace(std::make_pair(_name, result));
	return m_atlas.at(_name);
}

void SpriteAtlasBatcher::Begin()
{
	assert(!m_hasBegun);
	m_drawArray.clear();
	m_hasBegun = true;
}

void SpriteAtlasBatcher::Enque(AtlasSprite as)
{
	assert(m_hasBegun);
	auto asBounds = as.getGlobalBounds();

	sf::Vertex v0(sf::Vector2f(asBounds.left, asBounds.top), as.TC0());
	v0.color = as.getColor();
	m_drawArray.append(v0);

	sf::Vertex v1(sf::Vector2f(asBounds.left, asBounds.top+asBounds.height), as.TC1());
	v1.color = as.getColor();
	m_drawArray.append(v1);

	sf::Vertex v2(sf::Vector2f(asBounds.left+asBounds.width, asBounds.top+asBounds.height), as.TC2());
	v2.color = as.getColor();
	m_drawArray.append(v2);

	sf::Vertex v3(sf::Vector2f(asBounds.left+asBounds.width, asBounds.top), as.TC3());
	v3.color = as.getColor();
	m_drawArray.append(v3);

}

void SpriteAtlasBatcher::End()
{
	assert(m_hasBegun);
	if(m_textureDirty)
	{
		m_atlasTexture.display();
		m_textureDirty=false;
		// Debugging: write texture atlas to file
	//m_atlasTexture.getTexture().copyToImage().saveToFile("toto.png");

	}

	m_hasBegun = false;
}

void SpriteAtlasBatcher::draw(sf::RenderTarget& _target, sf::RenderStates _states) const
{
	assert(!m_hasBegun);
	if( m_drawArray.getVertexCount() > 0 )
	{
		_states.texture = &m_atlasTexture.getTexture();
		_target.draw(m_drawArray, _states);
	}
}