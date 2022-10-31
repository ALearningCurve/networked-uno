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

bool GameState::get_is_reversed() {
	return _is_reversed;
}

bool GameState::get_is_game_started() {
	return _is_game_started;
}

int GameState::get_next_player() {
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

bool GameState::is_game_over() {
	for (Player& player : _players) {
		if (player.get_hand().get_number_cards() == 0) {
			return true;
		}
	}

	return false;
}