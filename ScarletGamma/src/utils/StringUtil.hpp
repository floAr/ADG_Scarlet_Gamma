#pragma once

#include <string>
#include <vector>

namespace Utils {

	/// \brief Test two strings if they are equal ignoring the case.
	bool IStringEqual(const std::string& _str1, const std::string& _str2);

	/// \brief Test two strings if the first is 'smaller' than the second ignoring the case.
	bool IStringLess(const std::string& _str1, const std::string& _str2);

	/// \brief Test if a string contains another ignoring the case.
	bool IStringContains(const std::string& _string, const std::string& _subString);


	/// \brief Splits a string with a certain delimiter
	std::vector<std::string> &ISplitString(const std::string &s, char delim, std::vector<std::string> &elems);

	/// \brief Splits a string with a certain delimiter
	std::vector<std::string> ISplitString(const std::string &s, char delim);
} // namespace Utils