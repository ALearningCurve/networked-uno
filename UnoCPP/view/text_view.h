#pragma once
#include <string>
#include <iostream>
#include "../models/game_state.h"

class TextView {
	std::ostream& _out;
	void output_message(const std::string& msg);

public:
	TextView(std::ostream& out) : _out(out) {};

	void error(const std::string text);
	void info(const std::string text);
	void alert(const std::string text);

	std::string stringify_current_turn(GameState& game);
	std::string stringify_player(const Player& player);
	std::string stringify_card(const Card& card);
	std::string stringify_hand(const Hand& hand, const std::string& indent="");
};