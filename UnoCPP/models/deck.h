#pragma once

#include "card.h"
#include "hand.h"
#include <algorithm>
#include <random>
#include <chrono>
#include "../exceptions/exception.h"


class Deck
{
	std::vector<std::shared_ptr<Card>> _cards;
	void create_card_deck();
public:
	Deck(): Deck(2) {};
	Deck(int n) { create_n_card_decks(n); shuffle_deck(); };
	void shuffle_deck();
	void create_n_card_decks(int n);
	int get_card_count();
	Card& draw_card();
	void deal_hand(Hand& h, int n);
	const std::vector<std::shared_ptr<Card>>& get_deck();
};