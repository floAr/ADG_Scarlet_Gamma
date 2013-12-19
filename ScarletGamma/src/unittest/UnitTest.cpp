#include "UnitTests.hpp"

namespace UnitTest {

	// Declarations of the sub-test functions
	void TestFormulaParser();
	void TestObjects();
	void TestMap();

	void Run()
	{
		TestFormulaParser();
		TestObjects();
		TestMap();
	}	  

}