#pragma once

#include <string>

namespace Utils {

	/// \brief Test two strings if they are equal ignoring the case.
	bool IStringEqual(const std::string& _str1, const std::string& _str2);

	/// \brief Test if a string contains another ignoring the case.
	bool IStringContains(const std::string& _string, const std::string& _subString);

} // namespace Utils