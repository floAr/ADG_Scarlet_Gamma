#include "StringUtil.hpp"

#include <algorithm>

namespace Utils {

	bool IStringEqual(const std::string& _str1, const std::string& _str2)
	{
		if(_str1.size() != _str2.size()) {
			return false;
		}
		for(std::string::const_iterator c1 = _str1.begin(), c2 = _str2.begin(); c1 != _str1.end(); ++c1, ++c2) {
			if (tolower(*c1) != tolower(*c2)) {
				return false;
			}
		}
		return true;
	}

	bool IStringContains(const std::string& _string, const std::string& _subString)
	{
		std::string pattern(_subString);
		std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
		std::string string(_string);
		std::transform(string.begin(), string.end(), string.begin(), ::tolower);
		return string.find(pattern) != std::string::npos;
	}

} // namespace Utils