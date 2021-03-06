#include "core/Map.hpp"
#include "core/World.hpp"
#include "utils/Random.hpp"
#include "unittest/UnitTests.hpp"
#include "network/WorldMessages.hpp"
#include <iostream>
#include "Constants.hpp"
#include "core/PredefinedProperties.hpp"

using namespace std;
using namespace Core;

namespace UnitTest {
	
	void TestMap()
	{
		Network::MaskWorldMessage messageLock;

		// Create a map
		World* world = new Core::World();
		world->Init();
		MapID mapID = world->NewMap("Scarlet Square",10,10);
		Map* map = world->GetMap( mapID );

		// Create objects and put them to the map
		Utils::Random rnd(236);
		for( int y=0; y<10; ++y ) for( int x=0; x<10; ++x ){
			ObjectID objID = world->NewObject("media/noise_2.png");
			map->Add(objID, x, y, 1 );
		}
		const char* walls[3] = {"media/bar_hor.png", "media/bar_vert.png", "media/cross_big.png"};
		for( int i=0; i<16; ++i ) {
			ObjectID objID = world->NewObject(walls[rnd.Uniform(0,2)]);
			Object* obj = world->GetObject(objID);
			obj->Add( PROPERTY::OBSTACLE );
			obj->Add( PROPERTY::OCCLUDER );
			obj->SetColor(sf::Color(rnd.Uniform(0,255), rnd.Uniform(0,255), rnd.Uniform(0,255), 255));
			map->Add(objID, rnd.Uniform(0,9), rnd.Uniform(0,9), 2);
		}

		// Add one active object
		ObjectID objID = world->NewObject("media/char2.png");
		Object* obj = world->GetObject(objID);
		obj->Add( PROPERTY::NAME ).SetValue("ICH");
		obj->Add( PROPERTY::OWNER ).SetValue("ICH");
		obj->Add( PROPERTY::PLAYER ).SetValue("0");
		obj->Add( PROPERTY::TARGET ).SetValue("0:3");
		obj->Add( PROPERTY::INVENTORY );
		auto& path = obj->Add( PROPERTY::PATH );
		path.AddObject(world->GetObject(map->GetObjectsAt(0,0)[0]));
		path.AddObject(world->GetObject(map->GetObjectsAt(9,9)[0]));
		obj->SetColor(sf::Color(0, 155, 0, 155));
		map->Add(objID, 0, 2, 6);

		// Save
		try {
			Jo::Files::HDDFile file("saves/unittest.json", Jo::Files::HDDFile::ModeFlags( Jo::Files::HDDFile::CREATE_FILE | Jo::Files::HDDFile::OVERWRITE ));
			world->Save( file );
		} catch(std::string _e) {TEST_FAILED(_e);}
		catch(...) {TEST_FAILED("Saving a world.");}
		delete world;

		// Load
		world = new Core::World();
		Jo::Files::HDDFile file("saves/unittest.json");
		world->Load( file );
		// Test sample
		if( world->GetObject( world->GetMap(mapID)->GetObjectsAt(2,2)[0] )->GetProperty("X").Value() != "2.000000" )
			TEST_FAILED("Could not load the world correct.");
		delete world;
	}

} // namespace UnitTest