#include "text_view.h"

void TextView::error(const std::string text)
{
	output_message("[X] " + text);
}

void TextView::info(const std::string text)
{
	output_message("[ ] " + text);
}

void TextView::alert(const std::string text)
{
	output_message("[~] " + text);
}

void TextView::output_message(const std::string& msg)
{
	_out << msg << std::endl;
}

std::string TextView::stringify_current_turn(GameState& game)
{
	std::stringstream ss;
	ss << "Current Game State:"; 
	ss << "\n\tLast played card: " << stringify_card(*game.get_last_card());
	ss << "\n\tYour hand: ";
	ss << "\n" << stringify_hand(game.get_current_player()->get_hand(), "\t\t");
	ss << "\n\tPlayer info:";
	for (Player* player : game.get_players()) {
		ss << "\n\t\t" << player->get_name() << ": with " << std::to_string(player->get_hand().get_number_cards()) << " cards";
	}
	return ss.str();
}

std::string TextView::stringify_player(const Player& player)
{
	return player.get_name();
}

std::string TextView::stringify_card(const Card& card)
{
	std::stringstream ss;
	ss << "<" << card.color() << " " << card.type() + ">";
	std::string s = ss.str();
	return s;
}

std::string TextView::stringify_hand(const Hand& hand, const std::string& indent)
{
	std::stringstream ss;
	for (const std::shared_ptr<Card> card : hand.cards()) {
		ss << indent << stringify_card(*card);
		if (&card != &hand.cards().back()) {
			ss << "\n";
		}
	}
	return ss.str();
}