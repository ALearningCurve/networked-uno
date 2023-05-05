#include "card.h"

const int NUM_COLOR_CARD_TYPES = 13;
const std::string COLOR_CARD_TYPES[NUM_COLOR_CARD_TYPES] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "skip", "draw_two", "reverse" };
const int NUM_WILD_CARD_TYPES = 2;
const std::string WILD_CARD_TYPES[NUM_WILD_CARD_TYPES] = { "wild", "draw_four" };
const int NUM_CARD_COLORS = 4;
const std::string CARD_COLORS[NUM_CARD_COLORS] = { "red", "green", "blue", "yellow" };

Card::Card(std::string type, std::string color): _type(type), _color(color)
{
}

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

bool Card::playable_on_top_of(Card const& other) const
{
	return 		
		this->is_wild() ||
		this->_color == other._color ||
		this->_type == other._type;
}

int Card::calc_draw_amount() const
{
	if (_type == "draw_two") 
	{
		return 2;
	}
	else if (_type == "draw_four") 
	{
		return 4;
	} 
	else
	{
		return -1;
	}
}

bool Card::is_skip() const
{
	return _type == "skip";
}

bool Card::is_wild() const
{
	return _is_wild;
}

bool Card::is_reverse() const
{
	return _type == "reverse";
}

void Card::set_wild_color() const
{
	
}

const std::string& Card::color()
{
	return _color;
}

const std::string& Card::type()
{
	return _type;
}
