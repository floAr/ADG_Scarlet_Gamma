#include "StringUtil.hpp"

#include <algorithm>

namespace Utils {

	bool IStringEqual(const std::string& _str1, const std::string& _str2)
	{
		size_t len = _str1.size();
		if(len != _str2.size()) {
			return false;
		}
		const char* cstr1 = _str1.c_str();
		const char* cstr2 = _str2.c_str();
		for( size_t i=0; i<len; ++i )
		{
			if (tolower(cstr1[i]) != tolower(cstr2[i])) {
				return false;
			}
		}
		return true;
	}


	bool IStringLess(const std::string& _str1, const std::string& _str2)
	{
		size_t len = _str1.size();
		if(len != _str2.size()) {
			return false;
		}
		const char* cstr1 = _str1.c_str();
		const char* cstr2 = _str2.c_str();
		for( size_t i=0; i<len; ++i )
		{
			if (tolower(cstr1[i]) >= tolower(cstr2[i])) {
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