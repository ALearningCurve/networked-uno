#include "deck.h"

void Deck::create_card_deck() {

}

void Deck::shuffle_deck() {
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine rand(seed);
	std::random_shuffle(_cards.begin(), _cards.end(), rand);
}

void Deck::create_n_card_decks(int n) {
	while (n > 0) {
		create_card_deck();
		--n;
	}
}

int Deck::get_card_count() {
	return this->_cards.size();
}

const Card& Deck::draw_card() {
	if (get_card_count() == 0) {
		throw Ex("No more cards to draw");
	}
	const Card& back = _cards.back();
	_cards.pop_back();
	return back;
}

const std::vector<Card>& Deck::get_deck() {
	return _cards;
}

void Deck::deal_hand(Hand& hand, int n) {
	while (n > 0) {
		hand.add_card(draw_card());
		--n;
	}
}