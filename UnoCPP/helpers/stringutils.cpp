#include "stringutils.h"
#include <iostream>

void split_str(const std::string& str, const std::string& delim, std::vector<std::string>& strings)
{
    size_t start = 0;
    size_t end = str.find(delim);

    while (end != std::string::npos)
    {
        strings.push_back(str.substr(start, end - start));
        start = end + delim.length();
        end = str.find(delim, start);
    }

    strings.push_back(str.substr(start, end - start));
}

std::string lsplit_str(const std::string& str, const std::string& delim, const unsigned int& offset) {
    size_t start = offset;
    size_t end = str.find(delim);
    return str.substr(start, end - start);
}