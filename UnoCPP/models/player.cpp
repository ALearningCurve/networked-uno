#include "player.h"

Card& Player::play_card(Card& card){
	_hand.remove_card(card);
}

void Player::add_card(Card& card)
{
	_hand.add_card(card);
}

std::string Player::format() const {
	return _name;
}

const Hand& Player::get_hand() const {
	return _hand;
}