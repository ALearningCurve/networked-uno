#pragma once

#include <vector>
#include <string>

/// <summary>
/// Given a string, split the string by delimeter and insert them into a vector.
/// splits on all occurences of the delimeter
/// </summary>
/// <param name="str">the string to split</param>
/// <param name="delim">the delimeter</param>
/// <param name="strings">the vector to insert split strings into </param>
void split_str(const std::string& str, const std::string& delim, std::vector<std::string>& strings);

/// <summary>
/// Splits a given string by the delimeter. In essence creates a substring from [offeset, delimeter).
/// </summary>
/// <param name="str">the string to split</param>
/// <param name="delim">delimeter to split on. Defaults to " "</param>
/// <param name="offset">the index to start the split from</param>
/// <returns>the substring from [offeset, firstDelimeterIndex)</returns>
std::string lsplit_str(const std::string& str, const std::string& delim = " ", const unsigned int& offset = 0);
