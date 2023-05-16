#include "player.h"

//const Card& Player::play_card(const Card& card){
//	return _hand.remove_card(card);
//}

const std::shared_ptr<Card> Player::play_card(int card) {
	return _hand.remove_card(card);
}

void Player::add_card(std::shared_ptr<Card> card)
{
	if (_is_uno_immune) {
		_is_uno_immune = false;
	}
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

bool Player::set_uno_immunity(bool val)
{
	if (val && _hand.get_number_cards() != 1) {
		throw std::invalid_argument("Cannot given uno immunity if there is more than 1 card in this player's hand!");
	}
	return (_is_uno_immune = val);
}

bool Player::is_uno_vulnerable() const
{
	return !_is_uno_immune && _hand.get_number_cards() == 1;
}
