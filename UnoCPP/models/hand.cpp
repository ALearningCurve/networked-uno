#include "hand.h"

void Hand::add_card(const Card& c) {
	this->_cards.push_back(c);
}

const std::vector<Card>& Hand::cards() const {
	return this->_cards;
}

std::string Hand::format() const {
	std::string formatted = "";
	for (auto i = _cards.begin(); i != _cards.end(); ++i) {
		formatted.append((*i).format() + " ");
	}

	return formatted;
}