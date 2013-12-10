#include "core/Map.hpp"
#include "core/World.hpp"
#include "utils/Random.hpp"
#include "unittest/UnitTests.hpp"
#include <iostream>

using namespace std;

namespace UnitTest {

	void TestMap()
	{
		// Create a map
		Core::World* world = new Core::World();
		Core::MapID mapID = world->NewMap("Scarlet Square",10,10);
		Core::Map* map = world->GetMap( mapID );

		// Create objects and put them to the map
		Utils::Random rnd(236);
		for( int y=0; y<10; ++y ) for( int x=0; x<10; ++x ){
			Core::ObjectID objID = world->NewObject("media/noise_2.png");
			map->Add(objID, x, y, 0 );
		}
		const char* walls[3] = {"media/bar_hor.png", "media/bar_vert.png", "media/cross_big.png"};
		for( int i=0; i<16; ++i ) {
			Core::ObjectID objID = world->NewObject(walls[rnd.Uniform(0,2)]);
			Core::Object* obj = world->GetObject(objID);
			obj->Add(Core::Property("Obstacle",""));
			obj->SetColor(sf::Color(rnd.Uniform(0,255), rnd.Uniform(0,255), rnd.Uniform(0,255), 255));
			map->Add(objID, rnd.Uniform(0,9), rnd.Uniform(0,9), 1);
		}

		// Add one active object
		Core::ObjectID objID = world->NewObject("media/smile_2.png");
		Core::Object* obj = world->GetObject(objID);
		obj->Add(Core::Property("Target","0:3"));
		Core::ObjectList path;
		path.Add(map->GetObjectsAt(0,0)[0]);
		path.Add(map->GetObjectsAt(9,9)[0]);
		obj->Add(Core::Property("Path", path));
		obj->SetColor(sf::Color(0, 155, 0, 155));
		map->Add(objID, rnd.Uniform(0,9), rnd.Uniform(0,9), 4);

		// Save
		try {
			world->Save( "saves/unittest.json" );
		} catch(std::string _e) {TEST_FAILED(_e);}
		catch(...) {TEST_FAILED("Saving a world.");}
		delete world;

		// Load
		world = new Core::World();
		world->Load( "saves/unittest.json" );
		// Test sample
		if( world->GetObject( world->GetMap(mapID)->GetObjectsAt(2,2)[0] )->GetProperty("X").Value() != "2.000000" )
			TEST_FAILED("Could not load the world correct.");
		delete world;
	}

} // namespace UnitTest