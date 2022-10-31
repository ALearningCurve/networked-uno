#pragma once
#include "card.h"
#include <collection.h>
#include "../exceptions/exception.h"

class Hand {
	std::vector<Card> _cards;
public:
	Hand() {}
	Hand(std::vector<Card> cards) { _cards.swap(cards); }
	void add_card(const Card& c);
	const Card& remove_card(const Card& c);
	const Card& remove_card(int pos);
	const std::vector<Card>& cards() const;
	std::string format() const;
};