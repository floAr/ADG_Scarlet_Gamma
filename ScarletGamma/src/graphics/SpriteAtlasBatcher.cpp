#include "graphics/SpriteAtlasBatcher.hpp"

#include "Constants.hpp"

using namespace Graphics;

int Graphics::SpriteAtlasBatcher::ATLAS_ID=0;

int AtlasSprite::ID(){
	return m_id;
}


SpriteAtlasBatcher* SpriteAtlasBatcher::m_instance=0;
SpriteAtlasBatcher* SpriteAtlasBatcher::Instance(){
	if(m_instance==0)
	{
		m_instance=new SpriteAtlasBatcher();

	}
	return m_instance;
}

SpriteAtlasBatcher::SpriteAtlasBatcher():m_drawArray(sf::Quads),m_atlasBounds(0,0),m_hasBegun(false),m_yOffset(0){
	m_atlasTexture.create(64,64);
}
//
//AtlasSprite Graphics::SpriteAtlasBatcher::AddSpriteToAtlas(const std::string name,sf::Sprite sprite)
//{
//	if(m_atlas.count(name)){
//		return m_atlas.at(name);
//	}
//	AtlasSprite result(SpriteAtlasBatcher::ATLAS_ID++);
//	//result.setColor(sprite.getColor());
//	//result.setOrigin(sprite.getOrigin());
//	//result.setPosition(sprite.getPosition());
//	//result.setRotation(sprite.getRotation());
//	//result.setScale(sprite.getScale());
//	result.setTexture(*sprite.getTexture());
//	//result.setTextureRect(sprite.getTextureRect());
//
//	auto tex=sprite.getTexture();
//
//	if(m_atlasBounds.x+tex->getSize().x<m_atlasTexture.getSize().x)//object is still matching on x axsis
//	{
//
//		result.setPosition(m_atlasBounds);
//		result.SetTC(m_atlasBounds,m_atlasBounds+(sf::Vector2f)tex->getSize());
//		m_atlasTexture.draw(result);
//		m_atlasBounds.x+=tex->getSize().x;
//	}
//	else//start next line
//	{
//		m_atlasBounds.x=0;
//		m_atlasBounds.y+=100; //TODO : MAKE THIS CORRECT!
//
//		result.setPosition(m_atlasBounds);
//		result.SetTC(m_atlasBounds,m_atlasBounds+(sf::Vector2f)tex->getSize());
//
//		m_atlasTexture.draw(result);
//		m_atlasBounds.x+=tex->getSize().x;
//	}
//
//	m_atlas.emplace(std::make_pair(name,result));
//	return result;
//}


AtlasSprite Graphics::SpriteAtlasBatcher::AddOrGetAtlasSprite(const std::string name)
{
	if(m_atlas.count(name)){
		return m_atlas.at(name);
	}
	AtlasSprite result(SpriteAtlasBatcher::ATLAS_ID++);

	auto tex=Content::Instance()->LoadTexture(name);
	if(!(m_atlasBounds.x+tex.getSize().x<m_atlasTexture.getSize().x))//object is nolonger matching on x axsis
	{
		m_atlasBounds.x=0;
		m_atlasBounds.y+=m_yOffset;
		m_yOffset=0;
	}
	result.setTexture(tex,true);
	result.setPosition(m_atlasBounds);

	result.SetTC(m_atlasBounds,m_atlasBounds+(sf::Vector2f)tex.getSize());

	m_atlasTexture.draw(result);
	result.setScale(float(TILESIZE)/tex.getSize().x, float(TILESIZE)/tex.getSize().y);

	m_atlasBounds.x+=tex.getSize().x;
	if(tex.getSize().y>m_yOffset)
		m_yOffset=tex.getSize().y;
	

	m_atlas.emplace(std::make_pair(name,result));
	return result;
}

void SpriteAtlasBatcher::Begin(){
	assert(!m_hasBegun);
	m_drawArray.clear();
	m_drawArray.setPrimitiveType(sf::PrimitiveType::Quads);
	m_hasBegun=true;
}

void SpriteAtlasBatcher::Enque(AtlasSprite as){
	assert(m_hasBegun);
	auto asBounds=as.getGlobalBounds();

	sf::Vertex v0(sf::Vector2f(asBounds.left,asBounds.top),as.TC0());
	v0.color=as.getColor();
	m_drawArray.append(v0);

	sf::Vertex v1(sf::Vector2f(asBounds.left,asBounds.top+asBounds.height),as.TC1());
	v1.color=as.getColor();
	m_drawArray.append(v1);

	sf::Vertex v2(sf::Vector2f(asBounds.left+asBounds.width,asBounds.top+asBounds.height),as.TC2());
	v2.color=as.getColor();
	m_drawArray.append(v2);

	sf::Vertex v3(sf::Vector2f(asBounds.left+asBounds.width,asBounds.top),as.TC3());
	v3.color=as.getColor();
	m_drawArray.append(v3);

}

void SpriteAtlasBatcher::End(){
	assert(m_hasBegun);
	m_atlasTexture.display();
	//m_atlasTexture.getTexture().copyToImage().saveToFile("toto.png");
	m_hasBegun=false;
}

void SpriteAtlasBatcher::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	assert(!m_hasBegun);
	states.texture=&m_atlasTexture.getTexture();
	target.draw(m_drawArray,states);
	//target.draw(sf::Sprite(m_atlasTexture.getTexture()),states);
}