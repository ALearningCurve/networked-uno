#pragma once

#include "deck.h"
#include "player.h"
#include <string>
#include "card.h"

class GameState {
	std::vector<Player> _players;
	Deck _draw_deck;
	Deck _discard_deck;

	int _current_turn = 0;
	bool _is_reversed = false;
	std::string _color;
	int _draw_penalty = 0;
	GameState() {};
	const Card& draw_card();
public:
	GameState(Deck& deck, std::vector<Player> players) : _draw_deck(deck), _players(players) {};

	Player& get_current_player();
	const std::vector<Player>& get_players();
	void move_to_next_player();
	int get_next_player() const;
	bool get_is_reversed() const;
	void flip_direction();
	bool is_game_over() const;
	std::string get_color() const;
	void set_color(std::string color);
	const Card& drawForPlayer(Player& player); 
};