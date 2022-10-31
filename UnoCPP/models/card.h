#pragma once
#include <cstring>
#include <sstream>



class Card {
	std::string _type;
	std::string _color;
public:
	Card(std::string type, std::string color) : _type(type), _color(color){}
	std::string format() const;
	bool operator==(Card const& rhs) const;
};
