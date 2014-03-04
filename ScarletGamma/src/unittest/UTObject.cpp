#include "core/Object.hpp"
#include "Constants.hpp"
#include "unittest/UnitTests.hpp"
#include "network/WorldMessages.hpp"
#include <iostream>
#include "core/PredefinedProperties.hpp"
#include "utils/ValueInterpreter.hpp"
#include "utils/Random.hpp"

using namespace std;

namespace UnitTest {

	void TestObjects()
	{
		Network::MaskWorldMessage messageLock;

		// Create an object and add some stuff
		Core::Object obj(1, "floor.png");
		obj.Add( Core::PROPERTY::OBSTACLE );
		obj.Add( Core::PROPERTY::STRENGTH ).SetValue("1W8+10");
		obj.Add( Core::PROPERTY::INITIATIVE_MOD ).SetValue("5");
//		if( Utils::EvaluateFormula( "'Geschicklichkeit'+20", &Utils::Random(436), &obj ) != 25 )
//			TEST_FAILED("Evaluation of formula wrong.");
		obj.Add( Core::PROPERTY::DEXTERITY ).SetValue("'INI-Mod'+20");
		auto& inventory =  obj.Add( Core::PROPERTY::INVENTORY );
		inventory.AddObject( &obj );

		// Serialize and deserialize the object
		Jo::Files::MetaFileWrapper Wrapper;
		obj.Serialize( Wrapper.RootNode );

		// Take samples to test the restored object
		Core::Object deserialized( Wrapper.RootNode );
		if( deserialized.GetProperty(STR_PROP_STRENGTH).Value() != "1W8+10" )
			TEST_FAILED("Property not restored properly.");
		if( !deserialized.GetProperty(STR_PROP_INVENTORY).IsObjectList() )
			TEST_FAILED("Object list not restored.");
		if( deserialized.GetProperty(STR_PROP_INVENTORY).GetObjects()[0] != obj.ID() )
			TEST_FAILED("Object list corrupted.");
		if( deserialized.GetProperty(STR_PROP_DEXTERITY).Evaluate(&deserialized) != 25 )
			TEST_FAILED("Evaluation of formula wrong.");
	}
} // namespace UnitTest