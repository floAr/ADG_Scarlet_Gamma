#include "utils/ValueInterpreter.hpp"
#include "unittest/UnitTests.hpp"
#include "utils/Random.hpp"
#include <iostream>

using namespace std;

namespace UnitTest {

	void TestFormulaParser()
	{
		Utils::Random dicer(236);
		// Deterministic
		if( Utils::EvaluateFormula( "42", &dicer ) != 42 )
			TEST_FAILED("Evaluation of formula: '42' failed");
		if( Utils::EvaluateFormula( "4+2", &dicer ) != 6 )
			TEST_FAILED("Evaluation of formula: '4+2' failed");
		if( Utils::EvaluateFormula( "4*2+1", &dicer ) != 9 )
			TEST_FAILED("Evaluation of formula: '4*2+1' failed");
		if( Utils::EvaluateFormula( "1+4*2", &dicer ) != 9 )
			TEST_FAILED("Evaluation of formula: '1+4*2' failed");
		if( Utils::EvaluateFormula( "4*(2+1)", &dicer ) != 12 )
			TEST_FAILED("Evaluation of formula: '4*(2+1)' failed");

		// Non deterministic
		for( int s = 0; s<10; ++s )
		{
			if( Utils::EvaluateFormula( "W3", &dicer ) > 3 || Utils::EvaluateFormula( "W3", &dicer ) < 1 )
				TEST_FAILED("Evaluation of formula: 'W3' failed");
			if( Utils::EvaluateFormula( "2W5", &dicer ) > 10 || Utils::EvaluateFormula( "2W5", &dicer ) < 2)
				TEST_FAILED("Evaluation of formula: '2W5' failed");
			if( Utils::EvaluateFormula( "1W6+--5", &dicer ) > 11 || Utils::EvaluateFormula( "1W6+--5", &dicer ) < 6)
				TEST_FAILED("Evaluation of formula: '1W6+--5' failed");
			if( Utils::EvaluateFormula( "W6*w6", &dicer ) > 36 || Utils::EvaluateFormula( "W6*w6", &dicer ) < 1)
				TEST_FAILED("Evaluation of formula: 'W6*w6' failed");
			if( Utils::EvaluateFormula( "-W8", &dicer ) > -1 || Utils::EvaluateFormula( "-W8", &dicer ) < -8 )
				TEST_FAILED("Evaluation of formula: '-W8' failed");

			// The ultimating formula
			int x = Utils::EvaluateFormula( "(-W8/2+6W6-2)*5", &dicer );
			if( x < 0 || x > 170 ) TEST_FAILED("Evaluation of formula: '(-W8/2+6W6-2)*5' failed");
		}

		// Test false formulas
		try {
			Utils::EvaluateFormula( "-*8", &dicer );
			TEST_FAILED("Formula: '-*8' should have thrown an exception");
		} catch(...) {}
		try {
			Utils::EvaluateFormula( "((1)", &dicer );
			TEST_FAILED("Formula: '((1)' should have thrown an exception");
		} catch(...) {}
		try {
			Utils::EvaluateFormula( "2)", &dicer );
			TEST_FAILED("Formula: '2)' should have thrown an exception");
		} catch(...) {}
		try {
			Utils::EvaluateFormula( "34m68", &dicer );
			TEST_FAILED("Formula: '34m68' should have thrown an exception");
		} catch(...) {}
	}
} // namespace UnitTest