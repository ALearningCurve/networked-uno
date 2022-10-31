#include "card.h"

extern const std::string COLOR_CARD_TYPES[13] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Skip", "Draw Two", "Reverse" };
extern const std::string WILD_CARD_TYPES[2] = { "Wild", "Draw Four" };
extern const std::string CARD_COLORS[4] = { "red", "green", "blue", "yellow" };

std::string Card::format() const {
	std::stringstream ss;
	ss << this->_type << ", " << this->_color;
	std::string s = ss.str();
	return s;
}

bool Card::operator==(Card const& rhs) const
{
	return _type == rhs._type && _color == rhs._color;
}
