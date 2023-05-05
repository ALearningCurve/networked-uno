#pragma once
#include "card.h"
#include "../exceptions/exception.h"
#include <vector>

class Hand {
	std::vector<Card> _cards;
public:
	Hand() {}
	Hand(std::vector<Card> cards) { _cards.swap(cards); }
	void add_card(const Card& c);
	Card& remove_card(const Card& c);
	Card& remove_card(int pos);
	const Card& peek_card(int pos);
	const std::vector<Card>& cards() const;
	std::string format() const;
	int get_number_cards() const;
};