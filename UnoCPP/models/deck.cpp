#include "deck.h"
#include <iostream>

void Deck::create_card_deck() {
	for (int c = 0; c < NUM_CARD_COLORS; ++c) {
		for (int t = 0; t < NUM_COLOR_CARD_TYPES; ++t) {
			_cards.push_back(std::make_shared<Card>(COLOR_CARD_TYPES[t], CARD_COLORS[c]));
		}
	}
	for (int t = 0; t < NUM_WILD_CARD_TYPES; ++t) {
		_cards.push_back(std::make_shared<Card>("wild", WILD_CARD_TYPES[t]));
	}
}

void Deck::shuffle_deck() {
	unsigned seed = static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count());
	std::default_random_engine rand(seed);
	std::shuffle(_cards.begin(), _cards.end(), rand);
}

void Deck::create_n_card_decks(int n) {
	while (n > 0) {
		create_card_deck();
		--n;
	}
}

int Deck::get_card_count() {
	return static_cast<int>(_cards.size());
}

std::shared_ptr<Card> Deck::draw_card() {
	if (get_card_count() == 0) {
		throw Ex("No more cards to draw");
	}
	auto back = _cards.back();
	_cards.pop_back();
	return back;
}

const std::vector<std::shared_ptr<Card>>& Deck::get_deck() {
	return _cards;
}

void Deck::deal_hand(Hand& hand, int n) {
	while (n > 0) {
		hand.add_card(draw_card());
		--n;
	}
}