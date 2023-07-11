#pragma once
#include <string>
#include <vector>
#include <map>
#include <WinSock2.h>
#include <optional>
#include <exception>
#include "./game_state.h"

struct SocketPlayer {
	SOCKET socket;
	std::unique_ptr<Player> player;
};

class Lobby {
public:
	std::string _lobby_id;
	std::vector<SocketPlayer> _clients;
	std::unique_ptr<GameState> _game = nullptr;
	SOCKET _creator;
	// if is started, don't allow new clients to join
	bool _started = false;
	int _max_players;
	Lobby(std::string lobbyId, SOCKET creator, int maxPlayers) : _lobby_id(lobbyId), _creator(creator), _max_players(maxPlayers) {
		if (maxPlayers < 2 || maxPlayers > 4) {
			throw std::exception("Max Players must between 2 and 4");
		}
	}

	Player* get_player_from_socket(SOCKET s) {
		for (auto& client : _clients) {
			if (client.socket == s) {
				return client.player.get();
			}
		}
		throw std::exception("Could not find a player object in the lobby with the given SOCKET");
	}
};

class LobbyManager
{
	std::map<std::string, std::unique_ptr<Lobby>> _lobby_map;
	std::map<SOCKET, std::optional<std::string>> _client_map;

	void start_lobby_game(std::string& lobbyId);
public:
	void create_lobby(std::string identifier, SOCKET creator, int maxPlayers);
	Lobby& get_lobby(std::string& lobbyId);
	void add_player_to_lobby(std::string& lobbyId, SocketPlayer player);
	void remove_lobby(std::string& lobbyId);

	void remove_client(SOCKET client);
	void add_client(SOCKET client);
	std::optional<std::string> get_client_lobby_id(SOCKET client);
};

