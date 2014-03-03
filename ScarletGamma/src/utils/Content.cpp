#include "Content.hpp"
#include <jofilelib.hpp>
#include "Constants.hpp"
#include "network/ResourceMessages.hpp"

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


Content* Content::Instance(){
	static Content Instance;
	return &Instance;
}

const sf::Image& Content::LoadImage(const std::string& filename){
	auto it = m_img_cache.find(filename);
	if(it != m_img_cache.end())
	{
		return *it->second;
	}
	else
	{
		std::shared_ptr<sf::Image> res_handle = nullptr;
		if( Jo::Files::Utils::Exists( filename ) )
			res_handle = m_res_cache.acquire(thor::Resources::fromFile<sf::Image>(filename));
		if(res_handle)
		{
			CheckSynchronizable(filename);
			m_img_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			std::cerr<<"Error loading file "<<filename<<" using default object\n";
			return *(m_img_cache.find(DEFAULT_IMAGE))->second;
		}
	}	
}

const sf::Texture& Content::LoadTexture(const std::string& filename){
	bool isdefault(false);
	return LoadTexture(filename,&isdefault);
}

const sf::Texture& Content::LoadTexture(const std::string& filename,bool* isDefault_out){
	auto it = m_tex_cache.find(filename);
	if(it != m_tex_cache.end() && it->second != nullptr)
	{
		return *it->second;
	}
	else
	{
		std::shared_ptr<sf::Texture> res_handle = nullptr;
		if( Jo::Files::Utils::Exists( filename ) )
			res_handle = m_res_cache.acquire(thor::Resources::fromFile<sf::Texture>(filename));
		if(res_handle)
		{
			CheckSynchronizable(filename);
			m_tex_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			// Only output error once per file
			if( it == m_tex_cache.end() )
				std::cerr<<"Error loading file "<<filename<<" using default object\n";
			// Insert empty marker to avoid a bloat of messages
			m_tex_cache.insert(std::make_pair(filename,res_handle));
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
		std::shared_ptr<sf::Font> res_handle = nullptr;
		if( Jo::Files::Utils::Exists( filename ) )
			res_handle = m_res_cache.acquire(thor::Resources::fromFile<sf::Font>(filename));
		if(res_handle)
		{
			CheckSynchronizable(filename);
			m_fon_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			
			std::cerr<<"Error loading file "<<filename<<" using default object\n";
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
		std::shared_ptr<sf::Shader> res_handle = nullptr;
		if( Jo::Files::Utils::Exists( filename ) )
			res_handle = m_res_cache.acquire(thor::Resources::fromFile<sf::Shader>(filename,type));
		if(res_handle)
		{
			CheckSynchronizable(filename);
			m_sha_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			
			std::cerr<<"Error loading file "<<filename<<" using default object\n";
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
		std::shared_ptr<sf::SoundBuffer> res_handle = nullptr;
		if( Jo::Files::Utils::Exists( filename ) )
			res_handle = m_res_cache.acquire(thor::Resources::fromFile<sf::SoundBuffer>(filename));
		if(res_handle)
		{
			CheckSynchronizable(filename);
			m_sou_cache.insert(std::make_pair(filename,res_handle));
			return *res_handle;
		}
		else
		{
			std::cerr<<"Error loading file "<<filename<<" using default object\n";
			return *(m_sou_cache.find(DEFAULT_SOUNDBUFFER))->second;
		}
	}	
}


void Content::CheckSynchronizable(const std::string& _filename)
{
	// Find out if the resource is inside project directory
	std::string dir = Jo::Files::Utils::GetDirectory( _filename );
	if( !Jo::Files::Utils::IsEqual( dir, "media" ) )
	{
		throw STR_WRONG_PATH;
	}
}

/*void Content::RequestResource(const std::string& _filename)
{
	std::string dir = Jo::Files::Utils::GetDirectory( _filename );
	if( Jo::Files::Utils::IsEqual( dir, "media" ) )
	{
		// The resource could be existing in some other ones media directory.
		Network::MsgRequestResource(_filename).Send();
	} // else The given path cannot be synchronized.
}*/

void Content::Synchronize()
{
	// Go through the media directory and look for new files
	Jo::Files::Utils::FileEnumerator media("media");
	for( int i = 0; i < media.GetNumFiles(); ++i )
	{
		const std::string& name = media.GetFileName(i);
		if( m_mediaFiles.count( name ) == 0 )
		{
			// Add to list of my files
			m_mediaFiles.insert( name );
		}
	}

	// Send to everybody which files I have
	Network::MsgResourceList( m_mediaFiles ).Send();
}

void Content::Synchronize(const std::vector<std::string>& _reference)
{
	// Go through the reference list and look if there is something I do not have
	for( size_t i = 0; i < _reference.size(); ++i )
	{
		if( m_mediaFiles.count( _reference[i] ) == 0 )
		{
			m_mediaFiles.insert( _reference[i] );
			Network::MsgRequestResource( _reference[i] ).Send();
		}
	}
}