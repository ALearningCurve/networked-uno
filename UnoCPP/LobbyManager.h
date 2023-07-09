#pragma once
#include <string>
#include <vector>
#include <map>
#include <WinSock2.h>
#include <optional>
#include <exception>
#include "./models/game_state.h"

class Lobby {
public:
	std::string _lobbyId;
	std::vector<std::unique_ptr<SocketPlayer>> _clients;
	std::unique_ptr<GameState> _game = nullptr;
	SOCKET _creator;
	// if is started, don't allow new clients to join
	bool _started = false;
	int _maxPlayers;
	Lobby(std::string lobbyId, SOCKET creator, int maxPlayers) : _lobbyId(lobbyId), _creator(creator), _maxPlayers(maxPlayers) {
		if (maxPlayers < 2 || maxPlayers > 4) {
			throw std::exception("Max Players must between 2 and 4");
		}
	}
};

class LobbyManager
{
	std::map<std::string, std::unique_ptr<Lobby>> lobbyMap;
	std::map<SOCKET, std::optional<std::string>> clientMap;

	void startLobbyGame(std::string& lobbyId);
public:
	void createLobby(std::string identifier, SOCKET creator, int maxPlayers);
	Lobby& getLobby(std::string& lobbyId);
	void addPlayerToLobby(std::string& lobbyId, std::unique_ptr<SocketPlayer> player);
	void removeLobby(std::string& lobbyId);

	void removeClient(SOCKET client);
	void addClient(SOCKET client);
	std::optional<std::string> getClientLobbyId(SOCKET client);
};

