#pragma once
#include <string>
#include <iostream>
#include "../models/game_state.h"

class TextView {
	virtual void output_message(const std::string& msg) = 0;

public:
	void error(const std::string text);
	void info(const std::string text);
	void alert(const std::string text);

	static std::string stringify_current_turn(GameState& game);
	static std::string stringify_player(const Player& player);
	static std::string stringify_card(const Card& card);
	static std::string stringify_hand(const Hand& hand, const std::string& indent = "");
};

class StreamView: public TextView {
	std::ostream& _out;
	void output_message(const std::string& msg);

public:
	StreamView(std::ostream& out) : _out(out) {};
};