#include "hand.h"

void Hand::add_card(std::shared_ptr<Card> card) {
	this->_cards.push_back(card);
}

//Card& Hand::remove_card(const Card& card)
//{
//	auto it = std::find(_cards.begin(), _cards.end(), card);
//	if (it != _cards.end()) {
//		return *_cards.erase(it);
//	}
//	else {
//		throw Ex("Could play the desire card, it was not found");
//	}
//}

std::shared_ptr<Card> Hand::remove_card(int pos)
{
	// make sure that the index is valid
	if (pos < 0 || pos >= _cards.size()) {
		throw Ex("Card at that position is out of range");
	}

	// get the card from the vector and then return the card
	std::shared_ptr<Card> card = _cards.at(pos);
	_cards.erase(std::next(_cards.begin(), pos));;
	return card;
}

const std::shared_ptr<Card> Hand::peek_card(int pos) {
	return _cards.at(pos);
}

const std::vector<std::shared_ptr<Card>>& Hand::cards() const {
	return this->_cards;
}

std::string Hand::format() const {
	std::string formatted = "";
	for (auto i = _cards.begin(); i != _cards.end(); ++i) {
		formatted.append((**i).format() + " ");
	}

	return formatted;
}

int Hand::get_number_cards() const {
	return static_cast<int>(_cards.size());
}