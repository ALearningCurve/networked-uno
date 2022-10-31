#pragma once
#include "card.h"
#include <collection.h>

class Hand {
	std::vector<Card> _cards;
public:
	Hand() {}
	Hand(std::vector<Card> cards) { _cards.swap(cards); }
	void add_card(const Card& c);
	void remove_card(const Card& c);
	void remove_card(int pos);
	const std::vector<Card>& cards() const;
	std::string format() const;
};