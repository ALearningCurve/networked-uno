#pragma once
#include <cstring>
#include <sstream>
#include <array>

// CONSTANTS FOR THE DIFFERENT PROPERTIES OF THE CARDS
const std::array<std::string, 13> COLOR_CARD_TYPES = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "skip", "draw_two", "reverse" };
const std::array<std::string, 2> WILD_CARD_TYPES = { "wild", "draw_four" };
const std::array<std::string, 4> CARD_COLORS = { "red", "green", "blue", "yellow" };

/// <summary>
/// Represents a Card in a game of Uno.
/// </summary>
class Card {
	/// <summary>
	/// Corresponds to a type defined in COLOR_CARD_TYPES or WILD_CARD_TYPES
	/// </summary>
	const std::string _type;
	/// <summary>
	/// Corresponds to a color in CARD_COLORS or "wild"
	/// </summary>
	const std::string _color;

	Card();
public:
	Card(std::string type, std::string color) : _type(type), _color(color) {};

	/// <summary>
	/// Determines if this card equals another card if they both have the same type and color
	/// </summary>
	/// <param name="rhs">Card to compare to</param>
	/// <returns>True if this card equals the other card</returns>
	bool operator==(Card const &rhs) const;

	/// <summary>
	/// Determines if this card can be played on top the given card (same color, wild, or number) 
	/// </summary>
	/// <param name="other">the other card to compare to</param>
	/// <returns>True if this card can be played on the other card</returns>
	bool playable_on_top_of(Card const &other) const;

	/// <summary>
	/// Returns the number of cards that should be drawn if this card is played
	/// </summary>
	/// <returns>The amount to draw, or -1 if this card does not have a "draw" effect</returns>
	int calc_draw_amount() const;

	/// <summary>
	/// Returns if this card is a skip card
	/// </summary>
	/// <returns>true if this card is a skip card</returns>
	bool is_skip() const;

	/// <summary>
	/// Returns if this card is a wild card
	/// </summary>
	/// <returns>true if this card is a wild card</returns>
	bool is_wild() const;

	/// <summary>
	/// Returns if this card is a reverse card
	/// </summary>
	/// <returns>true if this card is a reverse card</returns>
	bool is_reverse() const;

	/// <summary>
	/// gets the color of the card
	/// </summary>
	/// <returns>color of the card as a string</returns>
	const std::string& color() const;

	/// <summary>
	/// gets the type of the card
	/// </summary>
	/// <returns>type of the card</returns>
	const std::string& type() const;
};
