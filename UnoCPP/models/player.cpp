#include "player.h"

const Card& Player::play_card(const Card& card){
	return _hand.remove_card(card);
}

const Card& Player::play_card(int card) {
	return _hand.remove_card(card);
}

void Player::add_card(const Card& card)
{
	_hand.add_card(card);
}


std::string Player::format() const {
	return _name;
}

const Hand& Player::get_hand() const {
	return _hand;
}

bool Player::is_bot() const
{
	return _is_bot;
}
