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
	
    sf::Image LoadImage(std::string filename);
	sf::Texture LoadTexture(std::string filename);
    sf::Font  LoadFont(std::string filename);
    sf::Shader LoadShader(std::string filename);
    sf::SoundBuffer LoadSoundBuffer(std::string filename);

};

