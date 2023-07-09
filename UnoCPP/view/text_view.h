#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "../models/game_state.h"
#include "../LobbyManager.h"
#include "../tcp_server.h"


class TextView {
	virtual void output_message(const std::string& msg) = 0;
public:
	void error(const std::string text);
	void info(const std::string text);
	void alert(const std::string text);
	void raw(const std::string text);

	static std::string stringify_current_turn(GameState& game);
	static std::string stringify_player(const Player& player);
	static std::string stringify_card(const Card& card);
	static std::string stringify_hand(const Hand& hand, const std::string& indent = "");
};

class StreamView: public TextView {
	std::ostream& _out;
	void output_message(const std::string& msg);

public:
	StreamView(std::ostream& out) : _out(out) {};
};

class SocketView : public TextView {
	std::vector<SOCKET>& _sockets;
	TcpServer& _server;
	void output_message(const std::string& msg);
public:
	SocketView(std::vector<SOCKET>& sockets, TcpServer& server) : _sockets(sockets), _server(server) { };
};

class DynamicClientLobbyView : public TextView {
	TcpServer& _server;
	LobbyManager& _lobbyManager;
	SOCKET& _client;
	void output_message(const std::string& msg);
public:
	DynamicClientLobbyView(LobbyManager& lobbyManager, TcpServer& server, SOCKET& client) : _lobbyManager(lobbyManager), _server(server), _client(client) { };
};
