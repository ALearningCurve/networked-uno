#pragma once

#include <vector>
#include <string>

std::vector<std::string>& split_str(const std::string& str, const std::string& delim = " ");

std::string lsplit_str(const std::string& str, const std::string& delim = " ", const unsigned int& offset = 0);