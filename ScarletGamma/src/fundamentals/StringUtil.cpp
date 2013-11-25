#include "StringUtil.hpp"

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

} // namespace Utils