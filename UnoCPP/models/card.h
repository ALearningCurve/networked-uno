#pragma once
#include <cstring>
#include <sstream>
#include <array>

const std::array<std::string, 13> COLOR_CARD_TYPES = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "skip", "draw_two", "reverse" };
const std::array<std::string, 2> WILD_CARD_TYPES = { "wild", "draw_four" };
const std::array<std::string, 4> CARD_COLORS = { "red", "green", "blue", "yellow" };

class Card {
	const std::string _type;
	const std::string _color;
	Card();
public:
	Card(std::string type, std::string color) : _type(type), _color(color) {};
	bool operator==(Card const &rhs) const;
	bool playable_on_top_of(Card const &other) const;
	int calc_draw_amount() const;
	bool is_skip() const;
	bool is_wild() const;
	bool is_reverse() const;
	const std::string& color() const;
	const std::string& type() const;
};
