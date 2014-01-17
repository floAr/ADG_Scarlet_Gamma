#include "core/Object.hpp"
#include "Constants.hpp"
#include "unittest/UnitTests.hpp"
#include "network/WorldMessages.hpp"
#include <iostream>

using namespace std;

namespace UnitTest {

	void TestObjects()
	{
		Network::MaskWorldMessage messageLock;

		// Create an object and add some stuff
		Core::Object obj(1, "floor.png");
		obj.Add( Core::Property(obj.ID(), Core::Property::R_V0EV0EV0E, string("Invisible"), string("true")) );
		obj.Add( Core::Property(obj.ID(), Core::Property::R_V0EV0EV0E, string("Inventory"), STR_EMPTY, Core::ObjectList()) );
		obj.Add( Core::Property(obj.ID(), Core::Property::R_V0EV0EV0E, string("Dmg"), string("1W8+10")) );
		auto& inventory = obj.GetProperty(string("Inventory"));
		inventory.AddObject( 2 );	// Add a probably wrong id

		// Serialize and deserialize the object
		Jo::Files::MetaFileWrapper Wrapper;
		obj.Serialize( Wrapper.RootNode );

		// Take samples to test the restored object
		Core::Object deserialized( Wrapper.RootNode );
		if( deserialized.GetProperty(string("Dmg")).Value() != "1W8+10" )
			TEST_FAILED("Property not restored properly.");
		if( !deserialized.GetProperty(string("Inventory")).IsObjectList() )
			TEST_FAILED("Object list not restored.");
		if( deserialized.GetProperty(string("Inventory")).GetObjects()[0] != 2 )
			TEST_FAILED("Object list corrupted.");
	}
} // namespace UnitTest