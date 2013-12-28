#include "Content.hpp"


Content::Content(){

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

sf::Image Content::LoadImage(std::string filename){
	if(m_img_cache.find(filename)!=m_img_cache.end())
	{
		return *m_img_cache[filename];		
	}
	else
	{
		std::shared_ptr<sf::Image> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::Image>(filename));
		m_img_cache.insert(std::make_pair(filename,res_handle));
		return *res_handle;
	}	
}

sf::Texture Content::LoadTexture(std::string filename){
	if(m_tex_cache.find(filename)!=m_tex_cache.end())
	{
		return *m_tex_cache[filename];		
	}
	else
	{
		std::shared_ptr<sf::Texture> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::Texture>(filename));
		m_tex_cache.insert(std::make_pair(filename,res_handle));
		return *res_handle;
	}	
}

sf::Font Content::LoadFont(std::string filename){
	if(m_fon_cache.find(filename)!=m_fon_cache.end())
	{
		return *m_fon_cache[filename];		
	}
	else
	{
		std::shared_ptr<sf::Font> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::Font>(filename));
		m_fon_cache.insert(std::make_pair(filename,res_handle));
		return *res_handle;
	}	
}

//sf::Shader Content::LoadShader(std::string filename){
//	if(m_sha_cache.find(filename)!=m_sha_cache.end())
//	{
//		return *m_sha_cache[filename];		
//	}
//	else
//	{
//		std::shared_ptr<sf::Shader> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::Shader>(filename));
//		m_sha_cache.insert(std::make_pair(filename,res_handle));
//		return *res_handle;
//	}	
//}

sf::SoundBuffer Content::LoadSoundBuffer(std::string filename){
	if(m_sou_cache.find(filename)!=m_sou_cache.end())
	{
		return *m_sou_cache[filename];		
	}
	else
	{
		std::shared_ptr<sf::SoundBuffer> res_handle=m_res_cache.acquire(thor::Resources::fromFile<sf::SoundBuffer>(filename));
		m_sou_cache.insert(std::make_pair(filename,res_handle));
		return *res_handle;
	}	
}

