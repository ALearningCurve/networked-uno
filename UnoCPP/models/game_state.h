#pragma once

#include "deck.h"
#include "player.h"

class GameState {
	std::vector<Player> _players;
	Deck _deck;
	int _current_turn = 0;
	bool _is_game_started = false;
	bool _is_reversed = false;

	// TODO something to keep track of the last card played

public:
	GameState() {};
	GameState(Deck& deck, std::vector<Player> players) : _deck(deck), _players(players) {};
	void add_player(Player& player);
	const Player& get_current_player();
	const std::vector<Player>& get_players();
	int get_next_player();
	bool get_is_reversed();
	bool get_is_game_started();
	bool is_game_over();
};