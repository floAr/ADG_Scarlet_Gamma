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
	/// \brief Loads a new image from file
	/// \param [in] filename	The filename of the object relative to root folder
    const sf::Image& LoadImage(const std::string& filename);
	/// \brief Loads a new texture from file
	/// \param [in] filename	The filename of the object relative to root folder
	const sf::Texture& LoadTexture(const std::string& filename);
    /// \brief Loads a new font from file
	/// \param [in] filename	The filename of the object relative to root folder
	const sf::Font&  LoadFont(const std::string& filename);
    /// \brief Loads a new shader from file [currently not working due restrictions on shader]
	/// \param [in] filename	The filename of the object relative to root folder
	const sf::Shader& LoadShader(const std::string& filename);
    /// \brief Loads a new sound buffer from file
	/// \param [in] filename	The filename of the object relative to root folder
	const sf::SoundBuffer& LoadSoundBuffer(const std::string& filename);

};

