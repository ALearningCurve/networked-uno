#pragma once

#include <vector>
#include <string>


void split_str(const std::string& str, const std::string& delim, std::vector<std::string>& strings);

std::string lsplit_str(const std::string& str, const std::string& delim = " ", const unsigned int& offset = 0);
