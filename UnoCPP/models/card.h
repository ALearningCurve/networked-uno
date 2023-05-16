#pragma once
#include <cstring>
#include <sstream>
#include <array>

const std::array<std::string, 13> COLOR_CARD_TYPES = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "skip", "draw_two", "reverse" };
const std::array<std::string, 2> WILD_CARD_TYPES = { "wild", "draw_four" };
const std::array<std::string, 4> CARD_COLORS = { "red", "green", "blue", "yellow" };

class Card {
	std::string _type;
	std::string _color;
	Card();
public:
	Card(std::string type, std::string color);
	std::string format() const;
	bool operator==(Card const &rhs) const;
	/// <summary>
	/// Whether or not THIS card can be played on top of the other card. This card can be played on other card if either
	///  this card has same color, this card has same number, or this card is wild.
	/// </summary>
	/// <param name="other">the card to check if this card can be played on top of</param>
	/// <returns>whether or not it can be played on top of the other card</returns>
	bool playable_on_top_of(Card const &other) const;
	int calc_draw_amount() const;
	bool is_skip() const;
	bool is_wild() const;
	bool is_reverse() const;
	const std::string& color();
	const std::string& type();
};
