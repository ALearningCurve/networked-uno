#include "LobbyManager.h"

void LobbyManager::createLobby(std::string identifier, SOCKET creator, int maxPlayers)
{
	if (this->lobbyMap.find(identifier) != lobbyMap.end()) {
		throw std::exception("This identifier has already been used");
	}
	if (maxPlayers < 2 || maxPlayers > 4) {
		throw std::exception("Could not create lobby, maxPlayers must be between 2 and 4 (inclusive)");
	}

	auto l = std::make_unique<Lobby>(identifier, creator, maxPlayers);
	lobbyMap[identifier] = std::move(l);
};

Lobby& LobbyManager::getLobby(std::string& lobbyId)
{
	auto found = lobbyMap.find(lobbyId);
	if (found == lobbyMap.end()) {
		throw std::exception("This identifier is not in use");
	}
	return *found->second;
}

void LobbyManager::addPlayerToLobby(std::string& lobbyId, std::unique_ptr<SocketPlayer> player)
{
	auto& lobby = getLobby(lobbyId);
	if (lobby._started || lobby._clients.size() >= lobby._maxPlayers) {
		std::stringstream ss;
		ss << "Could not join lobby with id \"" << lobbyId << "\" either because the game has already started or there are too many players";
		throw std::exception(ss.str().c_str());
	}

	for (auto& existingPlayer : lobby._clients) {
		if (existingPlayer->get_name() == player->get_name()) {
			std::stringstream ss;
			ss << "Player with the name \"" << player->get_name() << "\" already exists. Cannot add this player to lobby.";
			throw std::exception(ss.str().c_str());
		}
	}

	if (getClientLobbyId(player->getSocket()).has_value()) {
		throw std::exception("One client can only be in one lobby at a time, this client is already in a lobby!");
	}

	clientMap[player->getSocket()] = lobby._lobbyId;
	lobby._clients.push_back(std::move(player));
	if (lobby._clients.size() >= lobby._maxPlayers) {
		startLobbyGame(lobbyId);
	}
}

void LobbyManager::removeLobby(std::string& lobbyId)
{
	auto& lobby = getLobby(lobbyId);
	for (auto& client : lobby._clients) {
		auto iter = clientMap.find(client->getSocket());
		if (iter != clientMap.end()) {
			iter->second = std::nullopt;
		}
	}
	lobbyMap.erase(lobbyId);
}

void LobbyManager::removeClient(SOCKET client)
{
	auto optLobbyId = getClientLobbyId(client);
	if (optLobbyId.has_value()) {
		removeLobby(optLobbyId.value());
	}
	clientMap.erase(client);
}

void LobbyManager::addClient(SOCKET client)
{
	if (clientMap.find(client) != clientMap.end()) {
		throw std::exception("Cannot add two clients with the same ID");
	}
	// enter client into a map with no lobby id (""), which we use to mean no lobby ID
	clientMap[client] = std::nullopt;
}

std::optional<std::string> LobbyManager::getClientLobbyId(SOCKET client)
{
	auto clientIter = clientMap.find(client);
	if (clientIter == clientMap.end()) {
		throw std::exception("Could not find client to remove");
	}
	return clientIter->second;
}

void LobbyManager::startLobbyGame(std::string& lobbyId) {
	auto& lobby = getLobby(lobbyId);
	if (lobby._started) {
		throw std::exception("This game has already been started");
	}
	std::vector<Player*> players;
	for (auto& client : lobby._clients) {
		players.push_back(&(*client));
	}
	lobby._game = std::make_unique<GameState>(Deck(), players);
	lobby._started = true;
}
