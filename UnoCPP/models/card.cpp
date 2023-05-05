#include "card.h"

const int NUM_COLOR_CARD_TYPES = 13;
const std::string COLOR_CARD_TYPES[NUM_COLOR_CARD_TYPES] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "skip", "draw_two", "reverse" };
const int NUM_WILD_CARD_TYPES = 2;
const std::string WILD_CARD_TYPES[NUM_WILD_CARD_TYPES] = { "wild", "draw_four" };
const int NUM_CARD_COLORS = 4;
const std::string CARD_COLORS[NUM_CARD_COLORS] = { "red", "green", "blue", "yellow" };

std::string Card::format() const {
	std::stringstream ss;
	ss << this->_color << " " << this->_type;
	std::string s = ss.str();
	return s;
}

bool Card::operator==(Card const& rhs) const
{
	return _type == rhs._type && _color == rhs._color;
}

bool Card::canBePlayedOn(Card const& other) const
{
	if ()
	return false;
}
