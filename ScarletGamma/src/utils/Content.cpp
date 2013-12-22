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
		std::cout<<"newcontent";
		m_instance=new Content();

	}
	return m_instance;
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

