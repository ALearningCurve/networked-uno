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

void TextView::raw(const std::string text)
{
	output_message(text);
}

std::string TextView::stringify_game_start_for_player(GameState& game, Player* player)
{
	std::stringstream ss;
	ss << "Current Game State:"; 
	ss << "\n\tIts " << game.get_current_player()->get_name() << "'s turn";
	ss << "\n\tLast played card: ";
	ss << "" << stringify_card(*game.get_last_card());
	ss << "\n\tYour hand: ";
	ss << "\n" << stringify_hand(player->get_hand(), "\t\t");
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
	int counter = 0;
	for (const std::shared_ptr<Card> card : hand.cards()) {
		if (card != *hand.cards().begin()) {
			ss << '\n';
		}
		ss << indent << "[" << counter++ << "] " << stringify_card(*card);

	}
	return ss.str();
}

void StreamView::output_message(const std::string& msg)
{
	_out << msg << std::endl;
}

void SocketView::output_message(const std::string& msg)
{
	std::string to_output = msg + "\n";
	for (auto& socket : _sockets) {
		_server.send_client_message(socket, to_output);
	}
}

void DynamicClientLobbyView::output_message(const std::string& msg)
{
	auto optLobbyId = _lobby_manager.get_client_lobby_id(_client);
	if (!optLobbyId.has_value()) {
		return;
	}

	auto& lobby = _lobby_manager.get_lobby(optLobbyId.value());
	for (auto& client : lobby._clients) {
		std::vector<SOCKET> vec = { client.socket };
		SocketView view(vec, _server);
		view.raw(msg);
	}
}
