#pragma once

namespace UnitTest {

	/// \brief Run all implemented tests
	void Run();

	#define TEST_FAILED(msg) std::cerr << "FAILED: ["<< __FUNCTION__ <<":" << __LINE__ << "] " << msg << '\n';

} // namespace UnitTest