#include "Content.hpp"

/// \brief loads all the dummy items
Content::Content(){
	this->LoadImage(DEFAULT_IMAGE);
	this->LoadFont(DEFAULT_FONT);
	this->LoadTexture(DEFAULT_TEXTURE);
	//this->LoadSoundBuffer(DEFAULT_SOUNDBUFFER);
	//TODO: shader
	//this->LoadShader(DEFAULT_SHADER,sf::Shader::Type::Fragment);
}


Content::~Content(void)
{
}

Content* Content::m_instance=0;
Content* Content::Instance(){
	if(m_instance==0)
	{
		m_instance=new Content();

	}
	return m_instance;
}

const sf::Image& Content::LoadImage(const std::string& filename){
	auto it = m_img_cache.find(filename);
	if(it != m_img_cache.end())
	{
		return *it->second;
	}
	else
	{
		std::shared_ptr<sf::Image> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::Image>(filename));
		if(res_handle){
			m_img_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			return *(m_img_cache.find(DEFAULT_IMAGE))->second;
		}
	}	
}

const sf::Texture& Content::LoadTexture(const std::string& filename){
	bool isdefault(false);
	return Content::Instance()->LoadTexture(filename,&isdefault);
}

const sf::Texture& Content::LoadTexture(const std::string& filename,bool* isDefault_out){
	auto it = m_tex_cache.find(filename);
	if(it != m_tex_cache.end())
	{
		return *it->second;
	}
	else
	{
		std::shared_ptr<sf::Texture> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::Texture>(filename));
		if(res_handle){
			m_tex_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			*isDefault_out=true;
			return *(m_tex_cache.find(DEFAULT_TEXTURE))->second;
		}
	}	
}

const sf::Font& Content::LoadFont(const std::string& filename){
	auto it = m_fon_cache.find(filename);
	if(it != m_fon_cache.end())
	{
		return *it->second;
	}
	else
	{
		std::shared_ptr<sf::Font> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::Font>(filename));
		if(res_handle){
			m_fon_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			return *(m_fon_cache.find(DEFAULT_FONT))->second;
		}
	}	
}

const sf::Shader& Content::LoadShader(const std::string& filename,const sf::Shader::Type type){
	if(m_sha_cache.find(filename)!=m_sha_cache.end())
	{
		return *m_sha_cache[filename];		
	}
	else
	{
		std::shared_ptr<sf::Shader> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::Shader>(filename,type));
		if(res_handle){
			m_sha_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			return *(m_sha_cache.find(DEFAULT_SHADER))->second;
		}
	}	
}

const sf::SoundBuffer& Content::LoadSoundBuffer(const std::string& filename){
	auto it = m_sou_cache.find(filename);
	if(it != m_sou_cache.end())
	{
		return *it->second;
	}
	else
	{
		std::shared_ptr<sf::SoundBuffer> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::SoundBuffer>(filename));
		if(res_handle){
			m_sou_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			return *(m_sou_cache.find(DEFAULT_SOUNDBUFFER))->second;
		}
	}	
}

