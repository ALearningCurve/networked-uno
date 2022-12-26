#include "stringutils.h"

std::vector<std::string>& split_str(const std::string& str, const std::string& delim = " ")
{
	std::vector<std::string>(strings);

    std::string delim = " ";

    unsigned int start = 0;
    unsigned int end = str.find(delim);

    while (end != std::string::npos)
    {
        strings.push_back(str.substr(start, end - start));
        start = end + delim.length();
        end = str.find(delim, start);
    }

    strings.push_back(str.substr(start, end - start));

	return strings;
}
