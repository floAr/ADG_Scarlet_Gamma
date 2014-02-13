#include "World.hpp"
#include "Object.hpp"
#include "Map.hpp"
#include "network/WorldMessages.hpp"
#include "utils/StringUtil.hpp"
#include "Constants.hpp"
#include "PredefinedProperties.hpp"

using namespace std;

namespace Core {

	World::World() :
		m_propertyTemplates(-1)
	{
		// In an empty world the first possible ids are, well, the first available ids
		m_nextFreeMapID = 0;
		m_nextFreeObjectID = 0;
	}

	void World::Init()
	{
		CreateDefaultPropertyBase();
		CreateDefaultModuleBase();
		CreateDefaultTemplateBase();
	}

	Map* World::GetMap(MapID _id)
	{
		auto it = m_maps.find(_id);
		if( it == m_maps.end() ) return nullptr;
		else return &it->second;
		//return &m_maps.at(_id);
	}

	Object* World::GetObject(ObjectID _id)
	{
		auto it = m_objects.find(_id);
		if( it == m_objects.end() ) return nullptr;
		else return &it->second;
	}

	MapID World::NewMap( const std::string& _name, unsigned _sizeX, unsigned _sizeY )
	{
		m_maps.insert(std::make_pair<MapID,Map>(MapID(m_nextFreeMapID+0), Map(m_nextFreeMapID, _name, _sizeX, _sizeY, this)));
		Network::MsgAddMap( m_nextFreeMapID ).Send();
		++m_nextFreeMapID;
		return m_nextFreeMapID-1;
	}


	ObjectID World::NewObject( const std::string& _sprite )
	{
		m_objects.insert(std::make_pair<ObjectID,Object>(ObjectID(m_nextFreeObjectID+0), Object(m_nextFreeObjectID, _sprite)));
		Network::MsgAddObject( m_nextFreeObjectID ).Send();
		++m_nextFreeObjectID;
		return m_nextFreeObjectID-1;
	}

	MapID World::NewMap( const Jo::Files::MetaFileWrapper::Node& _node )
	{
		// Deserialize -> update maximum used MapID
		Map newMap(_node, this);
		m_nextFreeMapID = max(m_nextFreeMapID, newMap.ID());
		m_maps.insert(std::make_pair<MapID,Map>( newMap.ID(), std::move(newMap) ) );
		return newMap.ID();
	}


	ObjectID World::NewObject( Jo::Files::MetaFileWrapper::Node& _node, bool _newID )
	{
		// Deserialize -> update maximum used ObjectID or generate a new one
		if( _newID ) _node[STR_ID] = m_nextFreeObjectID++;
		Object newObj(_node);
		m_nextFreeObjectID = max(m_nextFreeObjectID, newObj.ID());
		m_objects.insert(std::make_pair<ObjectID,Object>( newObj.ID(), std::move(newObj) ) );

		// Insert to player list...
		RegisterObject(GetObject(newObj.ID()));

		return newObj.ID();
	}


	ObjectID World::NewObject( const Object* _object )
	{
		// Serialize old one and deserialize to new one
		Jo::Files::MetaFileWrapper wrapper;
		_object->Serialize( wrapper.RootNode );
		// The copy has a wrong id - give it a new one
		wrapper.RootNode[STR_ID] = m_nextFreeObjectID++;
		Object newObj( wrapper.RootNode );

		// Deep clone
		for( int i=0; i<newObj.GetNumElements(); ++i )
		{
			if( newObj.At(i)->IsObjectList() )
			{
				// Replace each entry in the list with a clone
				ObjectList& list = const_cast<ObjectList&>(newObj.At(i)->GetObjects());
				for( int j=0; j<list.Size(); ++j )
					list[j] = NewObject( GetObject(list[j]) );
			}
		}

		// Final insertion
		m_objects.insert(std::make_pair<ObjectID,Object>( newObj.ID(), std::move(newObj) ) );
		Network::MsgAddObject( newObj.ID() ).Send();
		return newObj.ID();
	}


	void World::Load( Jo::Files::IFile& _file )
	{
		// Clear old stuff
		m_maps.clear();
		m_objects.clear();
		m_players.clear();
		m_ownedObjects.clear();
		m_nextFreeMapID = 0;
		m_nextFreeObjectID = 0;

		// The save game is a top level file which contains maps and objects
		// serialized.
		Jo::Files::MetaFileWrapper saveGame(_file);

		// There should be an array of objects
		Jo::Files::MetaFileWrapper::Node* child;
		if( saveGame.RootNode.HasChild(string("Objects"), &child) )
		{
			// OK child should be an array of objects.
			for( unsigned i=0; i<child->Size(); ++i )
			{
				ObjectID id = NewObject( (*child)[i], false );
				Object* object = GetObject(id);
			}
		}

		// Do the same for the maps
		if( saveGame.RootNode.HasChild(string("Maps"), &child) )
		{
			// OK child should be an array of objects.
			for( unsigned i=0; i<child->Size(); ++i )
			{
				NewMap( (*child)[i] );
			}
		}

		// The nextFree...ID is now the maximum used id. The increment yields
		// the first really unused id.
		++m_nextFreeObjectID;
		++m_nextFreeMapID;

		// Load Property template object
		if( saveGame.RootNode.HasChild(string("PropertyObject"), &child) )
		{
			m_propertyTemplates = (ObjectID)*child;
		}// else CreateDefaultPropertyBase();

		if( saveGame.RootNode.HasChild(string("Modules"), &child) )
		{
			m_moduleTemplates = ObjectList( *child );
		}// else CreateDefaultModuleBase();

		if( saveGame.RootNode.HasChild(string("Templates"), &child) )
		{
			m_objectTemplates = ObjectList( *child );
		}// else CreateDefaultTemplateBase();
	}

	void World::Save( Jo::Files::IFile& _file ) const
	{
		Jo::Files::MetaFileWrapper saveGame;
		// Serialize objects
		auto& objects = saveGame.RootNode.Add(string("Objects"), Jo::Files::MetaFileWrapper::ElementType::NODE, m_objects.size());
		int i=0;
		for( auto it=m_objects.begin(); it!=m_objects.end(); ++it, ++i )
			it->second.Serialize(objects[i]);

		// Serialize maps
		auto& maps = saveGame.RootNode.Add(string("Maps"), Jo::Files::MetaFileWrapper::ElementType::NODE, m_maps.size());
		i=0;
		for( auto it=m_maps.begin(); it!=m_maps.end(); ++it, ++i )
			it->second.Serialize(maps[i]);

		// Serialize the template objects
		if( m_propertyTemplates != ObjectID(-1) )
			saveGame.RootNode[string("PropertyObject")] = m_propertyTemplates;
		if( m_moduleTemplates.Size() > 0 )
			m_moduleTemplates.Serialize( saveGame.RootNode[string("Modules")] );
		if( m_objectTemplates.Size() > 0 )
			m_objectTemplates.Serialize( saveGame.RootNode[string("Templates")] );

		// Write a new file
		saveGame.Write(_file, Jo::Files::Format::JSON);
	}

	void World::RemoveMap( MapID _map )
	{
		// Remove all objects referenced by the map
		{
			Network::MaskWorldMessage messageLock;
			Map* map = GetMap(_map);
			for( int y=map->Top(); y<=map->Bottom(); ++y )
				for( int x=map->Left(); x<=map->Right(); ++x )
				{
					ObjectList& list = map->GetObjectsAt(x,y);
					for( int i=0; i<list.Size(); ++i )
						RemoveObject( list[i] );
				}
		}
		Network::MsgRemoveMap( _map ).Send();
		// TODO: Test if destructor is called proper
		m_maps.erase( _map );
	}

	void World::RemoveObject( ObjectID _object )
	{
		Network::MsgRemoveObject( _object ).Send();
		// TODO: Test if destructor is called proper
		m_objects.erase( _object );
	}

	Object* World::FindPlayer( std::string _name )
	{
		// The player must not exists
		auto& it = m_players.find( _name );
		if( it != m_players.end() )
			return GetObject( it->second );
		return nullptr;
	}

	Object* World::FindPlayer( uint8_t _id )
	{
		for ( auto& it = m_players.begin(); it != m_players.end(); ++it)
		{
			Object* result = GetObject( it->second );
			// Using atoi is much faster than Evaluate
			if (atoi(result->Get(STR_PROP_PLAYER)->Value().c_str()) == _id)
				return result;
		}
		return nullptr;
	}

	

	std::vector<ObjectID> World::FilterObjectsByName( const std::string& _text ) const
	{
		std::vector<ObjectID> results;

		// For each object
		for( auto it=m_objects.begin(); it!=m_objects.end(); ++it )
		{
			// Test if the name contains the correct part
			if( Utils::IStringContains(it->second.GetName(), _text) )
				// Add reference to output
				results.push_back( it->second.ID() );
		}
		return results;
	}


	std::vector<MapID> World::GetAllMaps() const
	{
		// Enumerate all and store the ids.
		std::vector<MapID> results;
		for( auto it=m_maps.begin(); it!=m_maps.end(); ++it )
			results.push_back( it->second.ID() );
		return results;
	}


	std::vector<ObjectID> World::GetAllPlayers() const
	{
		// Enumerate all and store the ids.
		std::vector<ObjectID> results;
		for( auto it=m_players.begin(); it!=m_players.end(); ++it )
			results.push_back( it->second );
		return results;
	}


	void World::Update( float _dt )
	{
		for( auto it=m_maps.begin(); it!=m_maps.end(); ++it )
			it->second.Update( _dt );
	}


	ObjectID World::NewModuleTemplate( const std::string& _sprite )
	{
		ObjectID id = NewObject( _sprite );
		m_moduleTemplates.Add(id);
		return id;
	}


	ObjectID World::NewObjectTemplate( const std::string& _sprite )
	{
		ObjectID id = NewObject( _sprite );
		m_objectTemplates.Add(id);
		return id;
	}

	Object* World::GetNextObservableObject(ObjectID _currentID, int _direction)
	{
		if( m_ownedObjects.empty() ) return nullptr;

		assert( _direction == -1 || _direction == 1 );
		size_t currentIndex = std::find(m_ownedObjects.begin(), m_ownedObjects.end(), _currentID) - m_ownedObjects.begin();
		size_t next = currentIndex + _direction;
		next = (next + m_ownedObjects.size()) % m_ownedObjects.size();
		return GetObject( m_ownedObjects[next] );
	}

	void World::RegisterObject(Object* _object)
	{
		// Test object if it is a player and add it.
		if( _object->HasProperty( STR_PROP_PLAYER ) )
		{
			Property& prop = _object->GetProperty( STR_PROP_NAME );
			m_players[prop.Value()] = _object->ID();
		}
		// Test object if it has an owner and add it.
		if( _object->HasProperty( STR_PROP_OWNER ) )
		{
			Property& prop = _object->GetProperty( STR_PROP_OWNER );
			// Filter the template objects (they should all have owner = ""
			if( !prop.Value().empty() )
				m_ownedObjects.push_back(_object->ID());
		}
	}

	void World::UnregisterObject(ObjectID _object)
	{
		// Search in owned list to delete it
		for( size_t i=0; i<m_ownedObjects.size(); ++i )
		{
			if( m_ownedObjects[i] == _object )
			{
				// Remove by exchanging the element
				m_ownedObjects[i] = m_ownedObjects[m_ownedObjects.size()-1];
				m_ownedObjects.pop_back();
				--i;
			}
		}

		// Remove from player map
		m_players.erase( GetObject(_object)->GetProperty(STR_PROP_NAME).Value() );
	}

















	void World::CreateDefaultPropertyBase()
	{
		Network::MaskWorldMessage lock;
		// Create and fill an object with all known properties
		m_propertyTemplates = NewObject( STR_EMPTY );
		Object* propertyO = GetObject( m_propertyTemplates );

		propertyO->SetColor( sf::Color::White );
		propertyO->Add( PROPERTY::NAME );
		propertyO->Add( PROPERTY::OBSTACLE );
		propertyO->Add( PROPERTY::JUMPPOINT );
		propertyO->Add( PROPERTY::OWNER );

		propertyO->Add( PROPERTY::INVENTORY );
		propertyO->Add( PROPERTY::STRENGTH );
		propertyO->Add( PROPERTY::STRENGTH_MOD );
		propertyO->Add( PROPERTY::DEXTERITY );
		propertyO->Add( PROPERTY::DEXTERITY_MOD );
		propertyO->Add( PROPERTY::CONSTITUTION );
		propertyO->Add( PROPERTY::CONSTITUTION_MOD );
		propertyO->Add( PROPERTY::INTELLIGENCE );
		propertyO->Add( PROPERTY::INTELLIGENCE_MOD );
		propertyO->Add( PROPERTY::WISDOM );
		propertyO->Add( PROPERTY::WISDOM_MOD );
		propertyO->Add( PROPERTY::CHARISMA );
		propertyO->Add( PROPERTY::CHARISMA_MOD );
		propertyO->Add( PROPERTY::HEALTH );
		propertyO->Add( PROPERTY::HEALTH_MAX );
		propertyO->Add( PROPERTY::ARMORCLASS );
		propertyO->Add( PROPERTY::INITIATIVE_MOD );

		propertyO->Add( PROPERTY::ATTITUDE );
		propertyO->Add( PROPERTY::CLASS );
		propertyO->Add( PROPERTY::FOLK );
		propertyO->Add( PROPERTY::HOME );
		propertyO->Add( PROPERTY::FAITH );
		propertyO->Add( PROPERTY::SIZE );
		propertyO->Add( PROPERTY::AGE );
		propertyO->Add( PROPERTY::SEX );
		propertyO->Add( PROPERTY::WEIGHT );
		propertyO->Add( PROPERTY::HAIRCOLOR );
		propertyO->Add( PROPERTY::EYECOLOR );

		Object* talentO = GetObject( NewObject( STR_EMPTY ) );
		propertyO->Add( PROPERTY::TALENTS ).AddObject(talentO->ID());
		talentO->Add( PROPERTY::NAME ).SetValue( STR_PROP_TALENTS );
		talentO->GetProperty( STR_PROP_SPRITE ).SetRights( Property::R_SYSTEMONLY );
		talentO->GetProperty( STR_PROP_NAME ).SetRights( Property::R_SYSTEMONLY );
		talentO->Add( PROPERTY::ACROBATICS );
		talentO->Add( PROPERTY::DEMEANOR );
		talentO->Add( PROPERTY::PROFESSION );
		talentO->Add( PROPERTY::BLUFFING );
		talentO->Add( PROPERTY::DIPLOMACY );
		talentO->Add( PROPERTY::INTIMIDATE );
		talentO->Add( PROPERTY::UNLEASHING );
		talentO->Add( PROPERTY::PRESTIDIGITATION );
		talentO->Add( PROPERTY::FLYING );
		talentO->Add( PROPERTY::CRAFT );
		talentO->Add( PROPERTY::MEDICINE );
		talentO->Add( PROPERTY::STEALTH );
		talentO->Add( PROPERTY::CLIMBING );
		talentO->Add( PROPERTY::MAGICUSING );
		talentO->Add( PROPERTY::DEACTIVATE );
		talentO->Add( PROPERTY::ANIMALDEALING );
		talentO->Add( PROPERTY::MOTIVERECOGNITION );
		talentO->Add( PROPERTY::RIDING );
		talentO->Add( PROPERTY::ESTIMATE );
		talentO->Add( PROPERTY::SWIMMING );
		talentO->Add( PROPERTY::LANGUAGEUNDERSTANDING );
		talentO->Add( PROPERTY::SURVIVALING );
		talentO->Add( PROPERTY::MASQUERADING );
		talentO->Add( PROPERTY::PERCEPTION );
		talentO->Add( PROPERTY::KNOWLEDGE );
		talentO->Add( PROPERTY::MAGICART );
	}

	void World::CreateDefaultModuleBase()
	{
		Network::MaskWorldMessage lock;
		ObjectID OID;
		Object* object;

#		define NewModule(name)				\
			OID = NewObject( STR_EMPTY );	\
			m_moduleTemplates.Add( OID );	\
			object = GetObject( OID );		\
			object->Add( PROPERTY::NAME ).SetValue( name );	\
			object->GetProperty( STR_PROP_SPRITE ).SetRights( Property::R_SYSTEMONLY );	// Hide the sprite property

		NewModule( STR_ATTACKABLE );
		object->Add( PROPERTY::HEALTH );
		object->Add( PROPERTY::ARMORCLASS );

		NewModule( STR_JUMPPOINT );
		object->Add( PROPERTY::JUMPPOINT );

#		undef NewModule
	}

	void World::CreateDefaultTemplateBase()
	{
		Network::MaskWorldMessage lock;
		Object* object = GetObject( NewObjectTemplate( "media/gobbo.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_GOBBO );

		object = GetObject( NewObjectTemplate( "media/bar_hor.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_WALLH );
		object->Add( PROPERTY::OBSTACLE );
		object = GetObject( NewObjectTemplate( "media/bar_vert.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_WALLV );
		object->Add( PROPERTY::OBSTACLE );
		object = GetObject( NewObjectTemplate( "media/cross_big.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_WALLC );
		object->Add( PROPERTY::OBSTACLE );
		object = GetObject( NewObjectTemplate( "media/stairs.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_STAIRS );
		object->Add( PROPERTY::JUMPPOINT );
		object->Add( PROPERTY::COLOR ).SetValue( "777777ff" );
		object = GetObject( NewObjectTemplate( "media/planks.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_PLANKS );
		object->Add( PROPERTY::COLOR ).SetValue( "99aa44ff" );

		object = GetObject( NewObjectTemplate( "media/noise_2.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_EARTH );
		object->Add( PROPERTY::COLOR ).SetValue( "556622ff" );
		object = GetObject( NewObjectTemplate( "media/noise_2.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_GRASS );
		object->Add( PROPERTY::COLOR ).SetValue( "44bb44ff" );
		object = GetObject( NewObjectTemplate( "media/noise_1.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_WATER );
		object->Add( PROPERTY::OBSTACLE );
		object->Add( PROPERTY::COLOR ).SetValue( "aaaaeeff" );

	}

} // namespace Core