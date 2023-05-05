#include "game_state.h"

const Card& GameState::draw_card()
{
	if (_draw_deck.get_card_count() == 0) {
		if (_discard_deck.get_card_count() == 0) {
			_draw_deck.create_n_card_decks(1); // artificially extend if we run out of cards
		}
		std::swap(_draw_deck, _discard_deck);
	}
	return _draw_deck.draw_card();
}

GameState::GameState(Deck& deck, std::vector<Player> players): _draw_deck(deck), _players(players)
{
	if (deck.get_card_count() < 1) {
		throw std::invalid_argument("must be at least one card in the deck to start with");
	}
	else if (players.size() < 2) {
		throw std::invalid_argument("must be at least two players");
	}
	_last_card = &deck.draw_card();
}

Player& GameState::get_current_player() {
	return _players.at(_current_turn);
}

const std::vector<Player>& GameState::get_players() {
	return _players;
}

bool GameState::get_is_reversed() const {
	return _is_reversed;
}

void GameState::flip_direction() {
	_is_reversed = !_is_reversed;
}

void GameState::move_to_next_player()
{
	this->_current_turn = get_next_player();
}

int GameState::get_next_player() const {
	int dir = _is_reversed ? -1 : 1;
	int next_turn = _current_turn + dir;
	int size = static_cast<int>(_players.size());
	// implement logic to wrap the next turn around
	// to the player on the other side of the list
	// if the current turn is out of bounds
	if (next_turn < 0)
	{
		next_turn = size - 1;
	} 
	else if (next_turn >= size) {
		next_turn = 0;
	}

	return next_turn;
}

bool GameState::is_game_over() const {
	for (const Player& player : _players) {
		if (player.get_hand().get_number_cards() == 0) {
			return true;
		}
	}

	return false;
}

const Card& GameState::get_last_card() const {
	return *_last_card;
}

const Card& GameState::drawForPlayer(Player& player)
{
	const Card& card = this->draw_card();
	player.add_card(card);
	return card;
}

const void GameState::playCard(const int& cardPos)
{
	if (_draw_penalty == 0) 
	{
		throw std::invalid_argument("This player cannot play, they must skip!");
	}
	else if (_draw_penalty >= 0) 
	{
		throw std::invalid_argument("This player cannot play, they must draw cards!");
	}

	Player& player = get_current_player();
	Hand& hand = player.get_hand();
	if (cardPos < 0 || cardPos >= hand.get_number_cards()) {
		throw std::invalid_argument("Given card must be a value from 0 to" + std::to_string(hand.get_number_cards() - 1));
	}
	const Card& card = hand.peek_card(cardPos);
	if (card.playable_on_top_of(*_last_card)) {
		throw std::invalid_argument("Card must be playable on top of last card"); // TODO change state to keep track of last card
	}
	
	if (card.is_reverse()) {
		_is_reversed = !_is_reversed;
	}
	int _draw_penalty = card.calc_draw_amount();
	bool _next_is_skippped = card.is_skip();

	_last_card = &hand.remove_card(cardPos);
	return void();
}
