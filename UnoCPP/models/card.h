#pragma once
#include <cstring>
#include <sstream>

extern const int NUM_CARD_COLORS;
extern const int NUM_WILD_CARD_TYPES;
extern const int NUM_COLOR_CARD_TYPES ;
extern const std::string COLOR_CARD_TYPES[];
extern const std::string WILD_CARD_TYPES[];
extern const std::string CARD_COLORS[];

class Card {
	std::string _type;
	std::string _color;
public:
	Card(std::string type, std::string color = "red") : _type(type), _color(color) {}
	std::string format() const;
	bool operator==(Card const& rhs) const;
};
