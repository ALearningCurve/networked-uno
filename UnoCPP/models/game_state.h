#pragma once

#include "deck.h"
#include "player.h"
#include <string>
#include "card.h"
#include <optional>

class GameState {
	std::vector<Player*> _players;
	Deck _draw_deck;
	Deck _discard_deck;

	std::shared_ptr<Card> _last_card;
	int _current_turn = 0;
	bool _is_reversed = false;
	int _draw_penalty = 0;
	bool _next_is_skippped = false;

	GameState();
	std::shared_ptr<Card> draw_card();
public:
	GameState(Deck& deck, std::vector<Player*> players);
	Player* get_current_player();
	const std::vector<Player*>& get_players();
	void start_next_turn();
	int get_next_player() const;
	bool get_is_reversed() const;
	void flip_direction();
	const Player* get_winner() const;
	const std::shared_ptr<Card> get_last_card() const;
	const std::shared_ptr<Card> draw_for_player(Player* player);
	const void play_for_player(Player* player, const int& card, std::optional<std::string> optWildColor);
	const std::optional<std::string> can_play(Player* player, const int& card, std::optional<std::string> optWildColor);
	void player_said_uno(Player* player);
};