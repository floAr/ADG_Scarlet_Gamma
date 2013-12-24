#pragma once

#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <Thor/Resources.hpp>

#include <iostream>

#include <unordered_map>

class Content
{

public:
	static Content* Instance();

protected:
	Content();
private:
	static Content *m_instance;
	~Content(void);

	thor::MultiResourceCache m_res_cache;
	
	std::unordered_map<std::string,std::shared_ptr<sf::Image>> m_img_cache;
	std::unordered_map<std::string,std::shared_ptr<sf::Texture>> m_tex_cache;
	std::unordered_map<std::string,std::shared_ptr<sf::Font>> m_fon_cache;
	std::unordered_map<std::string,std::shared_ptr<sf::Shader>> m_sha_cache;
	std::unordered_map<std::string,std::shared_ptr<sf::SoundBuffer>> m_sou_cache;


public:
	
    const sf::Image& LoadImage(const std::string& filename);
	const sf::Texture& LoadTexture(const std::string& filename);
    const sf::Font&  LoadFont(const std::string& filename);
    const sf::Shader& LoadShader(const std::string& filename);
    const sf::SoundBuffer& LoadSoundBuffer(const std::string& filename);

};

