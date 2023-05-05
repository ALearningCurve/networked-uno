#pragma once

#include "deck.h"
#include "player.h"
#include <string>
#include "card.h"

class GameState {
	std::vector<Player> _players;
	Deck _draw_deck;
	Deck _discard_deck;

	Card* _last_card;

	int _current_turn = 0;
	bool _is_reversed = false;
	int _draw_penalty = 0;
	bool _next_is_skippped = false;
	GameState();
	const Card& draw_card();
public:
	GameState(Deck& deck, std::vector<Player> players);
	Player& get_current_player();
	const std::vector<Player>& get_players();
	void move_to_next_player();
	int get_next_player() const;
	bool get_is_reversed() const;
	void flip_direction();
	bool is_game_over() const;
	const Card& get_last_card() const;
	const Card& drawForPlayer(Player& player);
	const void playCard(const int& card);
};