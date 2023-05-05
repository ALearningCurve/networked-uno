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
	bool _is_wild = false;
public:
	Card(std::string type, std::string color = "red");
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
	void set_wild_color() const;
	const std::string& color();
	const std::string& type();
};
