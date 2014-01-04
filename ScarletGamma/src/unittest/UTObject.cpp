#include "core/Object.hpp"
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
		obj.Add( Core::Property(obj.ID(), string("Invisible"), string("true")) );
		obj.Add( Core::Property(obj.ID(), string("Inventory"), Core::ObjectList()) );
		obj.Add( Core::Property(obj.ID(), string("Dmg"), string("1W8+10")) );
		auto& inventory = obj.GetProperty(string("Inventory"));
		inventory.AddObject( 2 );	// Add a probably wrong id

		// Serialize and deserialize the object
		Jo::Files::MetaFileWrapper Wrapper;
		obj.Serialize( Wrapper.RootNode );

		// Take samples to test the restored object
		Core::Object deserialized( Wrapper.RootNode );
		if( deserialized.GetProperty(string("Dmg")).Evaluate() != 1.0 )
			TEST_FAILED("Property not restored properly.");
		if( !deserialized.GetProperty(string("Inventory")).IsObjectList() )
			TEST_FAILED("Object list not restored.");
		if( deserialized.GetProperty(string("Inventory")).GetObjects()[0] != 2 )
			TEST_FAILED("Object list corrupted.");
	}
} // namespace UnitTest