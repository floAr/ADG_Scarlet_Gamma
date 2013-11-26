#include "core/Object.hpp"
#include "unittest/UnitTests.hpp"
#include <iostream>

using namespace std;

namespace UnitTest {
	void TestObjects()
	{
		// Create an object and add some stuff
		Core::Object obj(1, 0.0f, 1.0f, "floor.png");
		obj.Add( Core::Property(string("Invisible"), string("true")) );
		obj.Add( Core::Property( string("Inventory"), Core::ObjectList() ) );
		auto& inventory = obj.GetProperty(string("Inventory"));
		inventory.Objects().Add( 2 );	// Add a probably wrong id

		// Serialize and deserialize the object
		Jo::Files::MetaFileWrapper Wrapper;
		obj.Serialize( Wrapper.RootNode );

		// Take samples to test the restored object
		Core::Object deserialized( Wrapper.RootNode );
		if( deserialized.GetProperty(string("Y")).Evaluate() != 1.0 )
			TEST_FAILED("Property not restored properly.");
		if( !deserialized.GetProperty(string("Inventory")).IsObjectList() )
			TEST_FAILED("Object list not restored.");
		if( deserialized.GetProperty(string("Inventory")).Objects()[0] != 2 )
			TEST_FAILED("Object list corrupted.");
	}
} // namespace UnitTest