#pragma once
#include <string>
#include <iostream>
class TextView {
	std::ostream& _out;
public:
	TextView(std::ostream& out) : _out(out) {};
	void output(std::string msg);
};