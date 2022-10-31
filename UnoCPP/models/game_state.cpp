#include "game_state.h"

void GameState::add_player(Player& player) {
	_players.push_back(player);
}

const Player& GameState::get_current_player() {
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

std::string GameState::get_color() const {
	return _color;
}

void GameState::set_color(std::string color) {
	bool valid = false;
	for (int i = 0; i < NUM_CARD_COLORS; ++i) {
		valid = valid || CARD_COLORS[i] == color;
	}
	if (valid) {
		_color = color;
	}
	else {
		throw Ex("Given a color that does not exist");
	}
}