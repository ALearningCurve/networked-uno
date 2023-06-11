#pragma once
#include "card.h"
#include "../exceptions/exception.h"
#include <vector>

class Hand {
	std::vector<std::shared_ptr<Card>> _cards;
public:
	Hand() {}
	Hand(std::vector<std::shared_ptr<Card>> cards) { _cards.swap(cards); }
	void add_card(std::shared_ptr<Card> c);
	//std::shared_ptr<Card> remove_card(const Card& c);
	std::shared_ptr<Card> remove_card(int pos);
	const std::shared_ptr<Card> peek_card(int pos);
	const std::vector<std::shared_ptr<Card>>& cards() const;
	int get_number_cards() const;
};