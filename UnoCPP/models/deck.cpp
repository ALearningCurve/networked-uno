#include "deck.h"
#include <iostream>

void Deck::create_card_deck() {
	/*originaldeck = [UnoCard(c, v) for v in ttypes for c in colors] + \
		[UnoCard('Wild', 'Wild') for _ in range(4)] + \
		[UnoCard('Wild', 'Draw Four') for _ in range(4)]*/

	for (int c = 0; c < NUM_CARD_COLORS; ++c) {
		for (int t = 0; t < NUM_COLOR_CARD_TYPES; ++t) {
			_cards.push_back(std::make_shared<Card>(COLOR_CARD_TYPES[t], CARD_COLORS[c]));
		}

		for (int t = 0; t < NUM_WILD_CARD_TYPES; ++t) {
			_cards.push_back(std::make_shared<Card>("Wild", WILD_CARD_TYPES[c]));
		}
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

Card& Deck::draw_card() {
	if (get_card_count() == 0) {
		throw Ex("No more cards to draw");
	}
	Card& back = *_cards.back();
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