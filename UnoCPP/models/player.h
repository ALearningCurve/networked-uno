#pragma once
#include <cstring>
#include <sstream>
#include "card.h"
#include "hand.h"
#include "../exceptions/exception.h"

class Player {
	std::string _name;
	Hand _hand;
public:
	Card& play_card(Card& card);
	void add_card(Card& card);
	std::string format() const;
	const Hand& get_hand() const;
};