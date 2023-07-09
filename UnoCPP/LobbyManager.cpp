#include "LobbyManager.h"

Lobby& LobbyManager::createLobby(std::string identifier, SOCKET creator)
{
	if (this->lobbyMap.find(identifier) != lobbyMap.end()) {
		throw std::exception("This identifier has already been used");
	}

	Lobby l;
	l.lobby_id = identifier;
	l.creator = creator;
	l.started = false;
	l.game = nullptr;
	return l;
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
	lobby.clients.push_back(std::move(player));
}

void LobbyManager::startLobbyGame(std::string& lobbyId, SOCKET requester) {
	auto& lobby = getLobby(lobbyId);
	if (requester != lobby.creator) {
		throw std::exception("This only the lobby creator can start the lobby");
	}
	if (lobby.started) {
		throw std::exception("This game has already been started");
	}

	lobby.game = std::make_unique<GameState>(Deck(), lobby.clients);
	lobby.started = true;
}
