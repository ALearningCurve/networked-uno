#pragma once
#include <cstring>
#include <sstream>
#include "card.h"
#include "hand.h"
#include "../exceptions/exception.h"

class Player {
	std::string _name;
	Hand _hand;
	bool _is_bot;
public:
	Player(std::string name) : _name(name), _is_bot(false) {};
	const Card& play_card(const Card& card);
	const Card& play_card(int card);
	void add_card(const Card& card);

	/**
	* Returns the player's name as a string
	*/
	std::string format() const;
	const Hand& get_hand() const;
	bool is_bot() const;
	const std::string& get_name() const;
};
