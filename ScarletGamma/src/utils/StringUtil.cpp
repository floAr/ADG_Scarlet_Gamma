#include "StringUtil.hpp"

#include <algorithm>
#include <sstream>

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
		size_t len = std::min(_str1.size(), _str2.size());
		const char* cstr1 = _str1.c_str();
		const char* cstr2 = _str2.c_str();
		for( size_t i=0; i<len; ++i )
		{
			char c1 = tolower(cstr1[i]);
			char c2 = tolower(cstr2[i]);
			if (c1 > c2) return false;
			else if(c1 < c2) return true;
			// Both where equal - continue search
		}
		// Beginning of both is equal. str1<str2 only if it is shorter
		return _str1.size() < _str2.size();
	}

	bool IStringContains(const std::string& _string, const std::string& _subString)
	{
		std::string pattern(_subString);
		std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
		std::string string(_string);
		std::transform(string.begin(), string.end(), string.begin(), ::tolower);
		return string.find(pattern) != std::string::npos;
	}


	std::vector<std::string> &ISplitString(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> ISplitString(const std::string &s, char delim) {
    std::vector<std::string> elems;
    ISplitString(s, delim, elems);
    return elems;
}

} // namespace Utils