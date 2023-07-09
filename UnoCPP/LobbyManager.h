#pragma once
#include <string>
#include <vector>
#include <map>
#include <WinSock2.h>
#include <optional>
#include "./models/game_state.h"

struct Lobby {
	std::string lobby_id;
	std::vector<std::unique_ptr<SocketPlayer>> clients;
	std::unique_ptr<GameState> game;
	SOCKET creator;
	// if is started, don't allow new clients to join
	bool started = false;
};

class LobbyManager
{
	std::map<std::string, std::unique_ptr<Lobby>> lobbyMap;
	void startLobbyGame(std::string& lobbyId, SOCKET requester);
public:
	Lobby& createLobby(std::string identifier, SOCKET creator);
	Lobby& getLobby(std::string& lobbyId);
	void addPlayerToLobby(std::string& lobbyId, std::unique_ptr<SocketPlayer> player);
	void removeLobby(std::string& lobbyId);
};

