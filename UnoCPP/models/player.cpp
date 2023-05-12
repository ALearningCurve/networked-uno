#include "player.h"

//const Card& Player::play_card(const Card& card){
//	return _hand.remove_card(card);
//}

const std::shared_ptr<Card> Player::play_card(int card) {
	return _hand.remove_card(card);
}

void Player::add_card(std::shared_ptr<Card> card)
{
	_hand.add_card(card);
}


std::string Player::format() const {
	return _name;
}

Hand& Player::get_hand() {
	return _hand;
}

const Hand& Player::get_hand() const {
	return _hand;
}

bool Player::is_bot() const
{
	return _is_bot;
}

const std::string& Player::get_name() const
{
	return this->_name;
}
