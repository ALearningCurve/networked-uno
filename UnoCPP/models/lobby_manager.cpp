#include "./lobby_manager.h"

void LobbyManager::create_lobby(std::string identifier, SOCKET creator, int maxPlayers)
{
	if (this->_lobby_map.find(identifier) != _lobby_map.end()) {
		throw std::exception("This identifier has already been used");
	}
	if (maxPlayers < 2 || maxPlayers > 4) {
		throw std::exception("Could not create lobby, maxPlayers must be between 2 and 4 (inclusive)");
	}

	auto l = std::make_unique<Lobby>(identifier, creator, maxPlayers);
	_lobby_map[identifier] = std::move(l);
};

Lobby& LobbyManager::get_lobby(std::string& lobbyId)
{
	auto found = _lobby_map.find(lobbyId);
	if (found == _lobby_map.end()) {
		throw std::exception("This identifier is not in use");
	}
	return *found->second;
}

void LobbyManager::add_player_to_lobby(std::string& lobbyId, SocketPlayer player)
{
	auto& lobby = get_lobby(lobbyId);
	if (lobby._started || lobby._clients.size() >= lobby._max_players) {
		std::stringstream ss;
		ss << "Could not join lobby with id \"" << lobbyId << "\" either because the game has already started or there are too many players";
		throw std::exception(ss.str().c_str());
	}

	for (auto& existingPlayer : lobby._clients) {
		if (existingPlayer.player->get_name() == player.player->get_name()) {
			std::stringstream ss;
			ss << "Player with the name \"" << player.player->get_name() << "\" already exists. Cannot add this player to lobby.";
			throw std::exception(ss.str().c_str());
		}
	}

	if (get_client_lobby_id(player.socket).has_value()) {
		throw std::exception("One client can only be in one lobby at a time, this client is already in a lobby!");
	}

	_client_map[player.socket] = lobby._lobbyId;
	lobby._clients.push_back(std::move(player));
	if (lobby._clients.size() >= lobby._max_players) {
		start_lobby_game(lobbyId);
	}
}

void LobbyManager::remove_lobby(std::string& lobbyId)
{
	auto& lobby = get_lobby(lobbyId);
	for (auto& client : lobby._clients) {
		auto iter = _client_map.find(client.socket);
		if (iter != _client_map.end()) {
			iter->second = std::nullopt;
		}
	}
	_lobby_map.erase(lobbyId);
}

void LobbyManager::remove_client(SOCKET client)
{
	auto optLobbyId = get_client_lobby_id(client);
	if (optLobbyId.has_value()) {
		remove_lobby(optLobbyId.value());
	}
	_client_map.erase(client);
}

void LobbyManager::add_client(SOCKET client)
{
	if (_client_map.find(client) != _client_map.end()) {
		throw std::exception("Cannot add two clients with the same ID");
	}
	// enter client into a map with no lobby id (""), which we use to mean no lobby ID
	_client_map[client] = std::nullopt;
}

std::optional<std::string> LobbyManager::get_client_lobby_id(SOCKET client)
{
	auto clientIter = _client_map.find(client);
	if (clientIter == _client_map.end()) {
		throw std::exception("Could not find client to remove");
	}
	return clientIter->second;
}

void LobbyManager::start_lobby_game(std::string& lobbyId) {
	auto& lobby = get_lobby(lobbyId);
	if (lobby._started) {
		throw std::exception("This game has already been started");
	}
	std::vector<Player*> players;
	for (auto& client : lobby._clients) {
		players.push_back(client.player.get());
	}
	lobby._game = std::make_unique<GameState>(Deck(), players);
	lobby._started = true;
}
