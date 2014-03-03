#include "World.hpp"
#include "Object.hpp"
#include "Map.hpp"
#include "network/WorldMessages.hpp"
#include "utils/StringUtil.hpp"
#include "Constants.hpp"
#include "PredefinedProperties.hpp"
#include <algorithm>

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
		// Lock, otherwise the insertion will cause a property to be send before the object
		Network::MaskObjectMessage lock;

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
		} else CreateDefaultPropertyBase();

		if( saveGame.RootNode.HasChild(string("Modules"), &child) )
		{
			m_moduleTemplates = ObjectList( *child );
		} else CreateDefaultModuleBase();

		if( saveGame.RootNode.HasChild(string("Templates"), &child) )
		{
			m_objectTemplates = ObjectList( *child );
		} else CreateDefaultTemplateBase();
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
		UnregisterObject( _object );
		// TODO: Test if destructor is called proper
		m_objects.erase( _object );
	}


	Object* World::FindPlayer( std::string _name )
	{
		// The player must not exists
		for ( auto& it = m_players.begin(); it != m_players.end(); ++it)
		{
			Object* result = GetObject( *it );
			if( result->GetName() == _name )
				return result;
		}
		return nullptr;
	}


	Object* World::FindPlayer( Core::PlayerID _id )
	{
		for ( auto& it = m_players.begin(); it != m_players.end(); ++it)
		{
			Object* result = GetObject( *it );
			// Using atoi is much faster than Evaluate
			Property& prop = result->GetProperty( STR_PROP_PLAYER );
			Core::PlayerID id = atoi(prop.Value().c_str());
			if( id == _id )
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


	const std::vector<ObjectID>& World::GetAllPlayers() const
	{
		return m_players;
	}


	void World::Update( float _dt )
	{
		if( !IsPaused() )
		{
			// Continue simulation of all maps (active identities)
			for( auto it=m_maps.begin(); it!=m_maps.end(); ++it )
				it->second.Update( _dt );
		}
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
		if( m_ownedObjects.empty()) return nullptr;
		assert( _direction == -1 || _direction == 1 );
		size_t currentIndex = std::find(m_ownedObjects.begin(), m_ownedObjects.end(), _currentID) - m_ownedObjects.begin();

		currentIndex = currentIndex + _direction;
		currentIndex = (currentIndex + m_ownedObjects.size()) % m_ownedObjects.size();
		return GetObject( m_ownedObjects[currentIndex] );
	}

	void World::RegisterObject(Object* _object)
	{
		// Test object if it is a player and add it.
		if( _object->HasProperty( STR_PROP_PLAYER ) )
		{
			// Avoid having the same object twice
			if( !m_players.empty() )
				for( auto it = m_players.begin(); it != m_players.end(); ++it )
					if( *it == _object->ID() )
					{
						m_players.erase(it);
						break;
					}
			m_players.push_back(_object->ID());
		}
		// Test object if it has an owner and add it.
		if( _object->HasProperty( STR_PROP_OWNER ) )
		{
			Property& prop = _object->GetProperty( STR_PROP_OWNER );
			// Filter the template objects (they should all have owner = "")
			// TODO: filter harder if templates get observed
			if( !prop.Value().empty() )
			{
				// Avoid multiple entries
				if ( std::count(m_ownedObjects.begin(), m_ownedObjects.end(), _object->ID()) == 0 )
					m_ownedObjects.push_back(_object->ID());
			}
			else
			{
				std::remove(m_ownedObjects.begin(), m_ownedObjects.end(), _object->ID());
			}
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
		if( GetObject(_object)->HasProperty(STR_PROP_PLAYER) )
			m_players.erase( std::find(m_players.begin(), m_players.end(), _object) );
	}


	bool World::IsPaused()
	{
		// Use a hidden property (which is synchronized automatically) to
		// communicate the pause.
		return GetPropertyBaseObject()->GetProperty(STR_PROP_PAUSE).Value() == STR_TRUE;
	}
	
	void World::SetPause( bool _pause )
	{
		// Use a hidden property (which is synchronized automatically) to
		// communicate the pause.
		if( _pause )
			GetPropertyBaseObject()->GetProperty(STR_PROP_PAUSE).SetValue( STR_TRUE );
		else GetPropertyBaseObject()->GetProperty(STR_PROP_PAUSE).SetValue( STR_FALSE );
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
		propertyO->Add( PROPERTY::SWITCH );
		propertyO->Add( PROPERTY::OWNER );
		propertyO->Add( PROPERTY::ITEM );
		propertyO->Add( PROPERTY::LAYER );
		propertyO->Add( PROPERTY::PAUSE );

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

		NewModule( STR_PROP_SWITCH );
		object->Add( PROPERTY::SWITCH );

		NewModule( STR_ATTRIBUTES );
		object->Add( PROPERTY::STRENGTH );
		object->Add( PROPERTY::DEXTERITY );
		object->Add( PROPERTY::CONSTITUTION );
		object->Add( PROPERTY::INTELLIGENCE );
		object->Add( PROPERTY::WISDOM );
		object->Add( PROPERTY::CHARISMA );
		object->Add( PROPERTY::STRENGTH_MOD );
		object->Add( PROPERTY::DEXTERITY_MOD );
		object->Add( PROPERTY::CONSTITUTION_MOD );
		object->Add( PROPERTY::INTELLIGENCE_MOD );
		object->Add( PROPERTY::WISDOM_MOD );
		object->Add( PROPERTY::CHARISMA_MOD );

		NewModule( STR_APPEARANCE );
		object->Add( PROPERTY::SIZE );
		object->Add( PROPERTY::AGE );
		object->Add( PROPERTY::SEX );
		object->Add( PROPERTY::WEIGHT );
		object->Add( PROPERTY::HAIRCOLOR );
		object->Add( PROPERTY::EYECOLOR );
		object->Add( PROPERTY::FOLK );

#		undef NewModule
	}

	void World::CreateDefaultTemplateBase()
	{
		Network::MaskWorldMessage lock;

        //////////////////////////////////////////////////////////////////////////
        // TEMPLATES
        Object* object = nullptr;

        // Goblin
		object = GetObject( NewObjectTemplate( "media/enemy3.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_GOBLIN_NAME );
        object->Add( PROPERTY::FOLK ).SetValue( STR_GOBLIN_NAME );
        object->Add( PROPERTY::EXPERIENCE ).SetValue( "135" );
        object->Add( PROPERTY::LAYER ).SetValue( STR_8 );
        object->Add( PROPERTY::COLOR ).SetValue( "33CC33ff" );
        object->Add( PROPERTY::SIZE ).SetValue( STR_ATT_SIZE_S );
        object->Add( PROPERTY::SPEED ).SetValue( "9" );
        object->Add( PROPERTY::PERCEPTION ).SetValue( "-1" );
        object->Add( PROPERTY::STRENGTH ).SetValue(      "11" );
        object->Add( PROPERTY::DEXTERITY ).SetValue(     "15" );
        object->Add( PROPERTY::CONSTITUTION ).SetValue(  "12" );
        object->Add( PROPERTY::INTELLIGENCE ).SetValue(  "10" );
        object->Add( PROPERTY::WISDOM ).SetValue(        "9" );
        object->Add( PROPERTY::CHARISMA ).SetValue(      "6" );
        object->Add( PROPERTY::STRENGTH_MOD );
        object->Add( PROPERTY::DEXTERITY_MOD );
        object->Add( PROPERTY::CONSTITUTION_MOD );
        object->Add( PROPERTY::INTELLIGENCE_MOD );
        object->Add( PROPERTY::WISDOM_MOD );
        object->Add( PROPERTY::CHARISMA_MOD );
        object->Add( PROPERTY::BASIC_ATTACK ).SetValue( "1" );
        object->Add( PROPERTY::RIDING ).SetValue( "14" );
        object->Add( PROPERTY::STEALTH ).SetValue( "14" );
        object->Add( PROPERTY::SWIMMING ).SetValue( "4" );
        object->Add( PROPERTY::ARMORCLASS ).SetValue( "2 + '" + STR_PROP_DEXTERITY_MOD + "' + 1 + 1");
        object->Add( PROPERTY::INITIATIVE_MOD ).SetValue( "'" + STR_PROP_DEXTERITY_MOD + "' + 4" );
        object->Add( PROPERTY::HEALTH ).SetValue( "1W10 + 1" );

        // Hobgoblin
        object = GetObject( NewObjectTemplate( "media/enemy1.png" ) );
        object->Add( PROPERTY::NAME ).SetValue( STR_HOBGOBLIN_NAME );
        object->Add( PROPERTY::FOLK ).SetValue( STR_HOBGOBLIN_NAME );
        object->Add( PROPERTY::EXPERIENCE ).SetValue( "200" );
        object->Add( PROPERTY::LAYER ).SetValue( STR_8 );
        object->Add( PROPERTY::COLOR ).SetValue( "118822ff" );
        object->Add( PROPERTY::SIZE ).SetValue( STR_ATT_SIZE_M );
        object->Add( PROPERTY::SPEED ).SetValue( "9" );
        object->Add( PROPERTY::PERCEPTION ).SetValue( "2" );
        object->Add( PROPERTY::STRENGTH ).SetValue(      "15" );
        object->Add( PROPERTY::DEXTERITY ).SetValue(     "15" );
        object->Add( PROPERTY::CONSTITUTION ).SetValue(  "16" );
        object->Add( PROPERTY::INTELLIGENCE ).SetValue(  "10" );
        object->Add( PROPERTY::WISDOM ).SetValue(        "12" );
        object->Add( PROPERTY::CHARISMA ).SetValue(      "8" );
        object->Add( PROPERTY::STRENGTH_MOD );
        object->Add( PROPERTY::DEXTERITY_MOD );
        object->Add( PROPERTY::CONSTITUTION_MOD );
        object->Add( PROPERTY::INTELLIGENCE_MOD );
        object->Add( PROPERTY::WISDOM_MOD );
        object->Add( PROPERTY::CHARISMA_MOD );
        object->Add( PROPERTY::BASIC_ATTACK ).SetValue( "1" );
        object->Add( PROPERTY::STEALTH ).SetValue( "9" );
        object->Add( PROPERTY::ARMORCLASS ).SetValue( "3 + '" + STR_PROP_DEXTERITY_MOD + "' + 1");
        object->Add( PROPERTY::INITIATIVE_MOD ).SetValue( "'" + STR_PROP_DEXTERITY_MOD + "'" );
        object->Add( PROPERTY::HEALTH ).SetValue( "1W10 + 7" );

        // Gnoll
        object = GetObject( NewObjectTemplate( "media/enemy1.png" ) );
        object->Add( PROPERTY::NAME ).SetValue( STR_GNOLL_NAME );
        object->Add( PROPERTY::FOLK ).SetValue( STR_GNOLL_NAME );
        object->Add( PROPERTY::EXPERIENCE ).SetValue( "400" );
        object->Add( PROPERTY::LAYER ).SetValue( STR_8 );
        object->Add( PROPERTY::COLOR ).SetValue( "6b501bff" );
        object->Add( PROPERTY::SIZE ).SetValue( STR_ATT_SIZE_M );
        object->Add( PROPERTY::SPEED ).SetValue( "9" );
        object->Add( PROPERTY::PERCEPTION ).SetValue( "2" );
        object->Add( PROPERTY::STRENGTH ).SetValue(      "15" );
        object->Add( PROPERTY::DEXTERITY ).SetValue(     "10" );
        object->Add( PROPERTY::CONSTITUTION ).SetValue(  "13" );
        object->Add( PROPERTY::INTELLIGENCE ).SetValue(  "8" );
        object->Add( PROPERTY::WISDOM ).SetValue(        "11" );
        object->Add( PROPERTY::CHARISMA ).SetValue(      "8" );
        object->Add( PROPERTY::STRENGTH_MOD );
        object->Add( PROPERTY::DEXTERITY_MOD );
        object->Add( PROPERTY::CONSTITUTION_MOD );
        object->Add( PROPERTY::INTELLIGENCE_MOD );
        object->Add( PROPERTY::WISDOM_MOD );
        object->Add( PROPERTY::CHARISMA_MOD );
        object->Add( PROPERTY::BASIC_ATTACK ).SetValue( "1" );
        object->Add( PROPERTY::ARMORCLASS ).SetValue( "2 + '" + STR_PROP_DEXTERITY_MOD + "' + 1 + 2");
        object->Add( PROPERTY::INITIATIVE_MOD ).SetValue( "'" + STR_PROP_DEXTERITY_MOD + "'" );
        object->Add( PROPERTY::HEALTH ).SetValue( "2W8 + 2" );

        // Kobold
        object = GetObject( NewObjectTemplate( "media/enemy1.png" ) );
        object->Add( PROPERTY::NAME ).SetValue( STR_KOBOLD_NAME );
        object->Add( PROPERTY::FOLK ).SetValue( STR_KOBOLD_NAME );
        object->Add( PROPERTY::EXPERIENCE ).SetValue( "100" );
        object->Add( PROPERTY::LAYER ).SetValue( STR_8 );
        object->Add( PROPERTY::COLOR ).SetValue( "446655ff" );
        object->Add( PROPERTY::SIZE ).SetValue( STR_ATT_SIZE_S );
        object->Add( PROPERTY::SPEED ).SetValue( "9" );
        object->Add( PROPERTY::PERCEPTION ).SetValue( "5" );
        object->Add( PROPERTY::STRENGTH ).SetValue(      "9" );
        object->Add( PROPERTY::DEXTERITY ).SetValue(     "13" );
        object->Add( PROPERTY::CONSTITUTION ).SetValue(  "10" );
        object->Add( PROPERTY::INTELLIGENCE ).SetValue(  "10" );
        object->Add( PROPERTY::WISDOM ).SetValue(        "9" );
        object->Add( PROPERTY::CHARISMA ).SetValue(      "8" );
        object->Add( PROPERTY::STRENGTH_MOD );
        object->Add( PROPERTY::DEXTERITY_MOD );
        object->Add( PROPERTY::CONSTITUTION_MOD );
        object->Add( PROPERTY::INTELLIGENCE_MOD );
        object->Add( PROPERTY::WISDOM_MOD );
        object->Add( PROPERTY::CHARISMA_MOD );
        object->Add( PROPERTY::BASIC_ATTACK ).SetValue( "1" );
        object->Add( PROPERTY::CRAFT).SetValue( "Fallenstellen, 6" );
        object->Add( PROPERTY::STEALTH).SetValue( "5" );
        object->Add( PROPERTY::ARMORCLASS ).SetValue( "2 + '" + STR_PROP_DEXTERITY_MOD + "' + 1 + 1");
        object->Add( PROPERTY::INITIATIVE_MOD ).SetValue( "'" + STR_PROP_DEXTERITY_MOD + "'" );
        object->Add( PROPERTY::HEALTH ).SetValue( "1W10" );

        // Lizardfolk
        object = GetObject( NewObjectTemplate( "media/enemy1.png" ) );
        object->Add( PROPERTY::NAME ).SetValue( STR_LIZARDFOLK_NAME );
        object->Add( PROPERTY::FOLK ).SetValue( STR_LIZARDFOLK_NAME );
        object->Add( PROPERTY::EXPERIENCE ).SetValue( "400" );
        object->Add( PROPERTY::LAYER ).SetValue( STR_8 );
        object->Add( PROPERTY::COLOR ).SetValue( "119977ff" );
        object->Add( PROPERTY::SIZE ).SetValue( STR_ATT_SIZE_M );
        object->Add( PROPERTY::SPEED ).SetValue( "9" );
        object->Add( PROPERTY::PERCEPTION ).SetValue( "1" );
        object->Add( PROPERTY::STRENGTH ).SetValue(      "13" );
        object->Add( PROPERTY::DEXTERITY ).SetValue(     "10" );
        object->Add( PROPERTY::CONSTITUTION ).SetValue(  "13" );
        object->Add( PROPERTY::INTELLIGENCE ).SetValue(  "9" );
        object->Add( PROPERTY::WISDOM ).SetValue(        "10" );
        object->Add( PROPERTY::CHARISMA ).SetValue(      "10" );
        object->Add( PROPERTY::STRENGTH_MOD );
        object->Add( PROPERTY::DEXTERITY_MOD );
        object->Add( PROPERTY::CONSTITUTION_MOD );
        object->Add( PROPERTY::INTELLIGENCE_MOD );
        object->Add( PROPERTY::WISDOM_MOD );
        object->Add( PROPERTY::CHARISMA_MOD );
        object->Add( PROPERTY::BASIC_ATTACK ).SetValue( "1" );
        object->Add( PROPERTY::ACROBATICS).SetValue( "6" );
        object->Add( PROPERTY::SWIMMING).SetValue( "8" );
        object->Add( PROPERTY::ARMORCLASS ).SetValue( "5 + '" + STR_PROP_DEXTERITY_MOD + "' + 2");
        object->Add( PROPERTY::INITIATIVE_MOD ).SetValue( "'" + STR_PROP_DEXTERITY_MOD + "'" );
        object->Add( PROPERTY::HEALTH ).SetValue( "2W8 + 2" );

        // Skeleton
        object = GetObject( NewObjectTemplate( "media/enemy2.png" ) );
        object->Add( PROPERTY::NAME ).SetValue( STR_SKELETON_NAME );
        object->Add( PROPERTY::FOLK ).SetValue( STR_SKELETON_NAME );
        object->Add( PROPERTY::EXPERIENCE ).SetValue( "135" );
        object->Add( PROPERTY::LAYER ).SetValue( STR_8 );
        object->Add( PROPERTY::COLOR ).SetValue( "888899ff" );
        object->Add( PROPERTY::SIZE ).SetValue( STR_ATT_SIZE_M );
        object->Add( PROPERTY::SPEED ).SetValue( "9" );
        object->Add( PROPERTY::PERCEPTION ).SetValue( "1" );
        object->Add( PROPERTY::STRENGTH ).SetValue(      "15" );
        object->Add( PROPERTY::DEXTERITY ).SetValue(     "14" );
        object->Add( PROPERTY::WISDOM ).SetValue(        "10" );
        object->Add( PROPERTY::CHARISMA ).SetValue(      "10" );
        object->Add( PROPERTY::STRENGTH_MOD );
        object->Add( PROPERTY::DEXTERITY_MOD );
        object->Add( PROPERTY::WISDOM_MOD );
        object->Add( PROPERTY::CHARISMA_MOD );
        object->Add( PROPERTY::BASIC_ATTACK ).SetValue( "0" );
        object->Add( PROPERTY::ARMORCLASS ).SetValue( "2 + '" + STR_PROP_DEXTERITY_MOD + "' + 2");
        object->Add( PROPERTY::INITIATIVE_MOD ).SetValue( "4 + '" + STR_PROP_DEXTERITY_MOD + "'" );
        object->Add( PROPERTY::HEALTH ).SetValue( "1W8" );

        // Treasure chest
        object = GetObject( NewObjectTemplate( "media/chest.png" ) );
        object->Add( PROPERTY::NAME ).SetValue( STR_CHEST_NAME );
        object->Add( PROPERTY::COLOR ).SetValue( "684f2bff");
        object->Add( PROPERTY::OBSTACLE );
		object->Add( PROPERTY::INVENTORY );

		// Well
		object = GetObject( NewObjectTemplate( "media/well.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_WELL_NAME );
		object->Add( PROPERTY::OBSTACLE );
		object->Add( PROPERTY::WATER );
		object->Add( PROPERTY::LAYER ).SetValue( STR_3 );


		object = GetObject( NewObjectTemplate( "media/bar_hor.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_WALLH );
		object->Add( PROPERTY::OBSTACLE );
		object->Add( PROPERTY::LAYER ).SetValue( STR_2 );
		object = GetObject( NewObjectTemplate( "media/bar_vert.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_WALLV );
		object->Add( PROPERTY::OBSTACLE );
		object->Add( PROPERTY::LAYER ).SetValue( STR_2 );
		object = GetObject( NewObjectTemplate( "media/cross_big.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_WALLC );
		object->Add( PROPERTY::OBSTACLE );
		object->Add( PROPERTY::LAYER ).SetValue( STR_2 );
		object = GetObject( NewObjectTemplate( "media/stairs.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_STAIRS );
		object->Add( PROPERTY::JUMPPOINT );
		object->Add( PROPERTY::LAYER ).SetValue( STR_2 );
		object->Add( PROPERTY::COLOR ).SetValue( "777777ff" );
		object = GetObject( NewObjectTemplate( "media/planks.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_PLANKS );
		object->Add( PROPERTY::COLOR ).SetValue( "b3995bff" );
		object->Add( PROPERTY::LAYER ).SetValue( STR_2 );

		object = GetObject( NewObjectTemplate( "media/noise_2.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_EARTH );
		object->Add( PROPERTY::COLOR ).SetValue( "9c7c44ff" );
		object->Add( PROPERTY::LAYER ).SetValue( STR_1 );
		object = GetObject( NewObjectTemplate( "media/noise_2.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_GRASS );
		object->Add( PROPERTY::COLOR ).SetValue( "607326ff" );
		object->Add( PROPERTY::LAYER ).SetValue( STR_1 );
		object = GetObject( NewObjectTemplate( "media/noise_1.png" ) );
		object->Add( PROPERTY::NAME ).SetValue( STR_WATER );
		object->Add( PROPERTY::OBSTACLE );
		object->Add( PROPERTY::COLOR ).SetValue( "61b9e2ff" );
		object->Add( PROPERTY::LAYER ).SetValue( STR_1 );

	}

} // namespace Core
