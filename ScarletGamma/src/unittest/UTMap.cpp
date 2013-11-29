#include "core/Map.hpp"
#include "core/World.hpp"
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
		for( int i=1; i<9; ++i ) {
			Core::ObjectID objID = world->NewObject("floor.png");
			map->Add(objID, i, i );
		}

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