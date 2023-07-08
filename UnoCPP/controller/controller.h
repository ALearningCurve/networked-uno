#pragma once
#include "../models/game_state.h"
#include "../models/player.h"
#include "../models/card.h"
#include "../view/text_view.h"
#include "../tcp_server.h"
#include <iostream>
#include <istream>
#include <map>
#include "command.h"
#include <memory>
#include <functional>
#include <winsock2.h>

using VecCommand = std::function<std::shared_ptr<TextCommand>(const std::vector<std::string>&)>;

class Controller {
public:
	virtual ~Controller() = default;
	virtual void startGame() = 0;
};

class SocketBasedController : public Controller {
public:
	virtual ~SocketBasedController() = default;
	virtual void onDisconnect(SOCKET s) = 0;
	virtual void onClientConnected(SOCKET s) = 0;
	virtual void onInputRecieved(SOCKET s, std::string) = 0;
};

// For local multiplayer
class TextController : Controller {
	GameState& _model;
	TextView* _view;
	std::istream& _input;
	bool _quit = false;

	std::map<std::string, VecCommand> make_dict();

	const std::map<std::string, VecCommand> _command_dict = make_dict();

	const void playerDoTurn();

public:
	TextController(GameState& model, TextView* view, std::istream& istream) : _model(model), _view(view), _input(istream) {};

	void startGame();
};

class SimpleSocketBasedController : public SocketBasedController {
	struct Lobby {
		std::string lobby_id;
		std::vector<SocketPlayer> clients;
		GameState game;
		SOCKET creator;
		// if is started, don't allow new clients to join
		bool started = false;
	};

	// invariant: if ClientInformation in client map has lobby_id, then
	// that lobby must exist in lobby map
	std::map<SOCKET, std::string> clientMap;
	std::map<std::string, std::unique_ptr<Lobby>> lobbyMap;
	TcpServer server;
public:
	/// <summary>
	/// Starts the networked server
	/// </summary>
	void startGame();

	/// <summary>
	/// handles the event of a client disconnecting from the server.
	/// If client disconnects and is in a lobby, it will end that lobby.
	/// </summary>
	/// <param name="s">socket of the disconnected client</param>
	void onDisconnect(SOCKET s);

	/// <summary>
	/// handles the event of a client connecting to the server
	/// </summary>
	/// <param name="s">socket of the conntected client</param>
	void onClientConnected(SOCKET s);

	/// <summary>
	/// handles the event of a client sending data to the server.
	/// Two unique cases:
	///		- If the client is not in a lobby, they must either start or join a lobby
	///		- If the client is in a lobby, they must play the game until completion
	/// </summary>
	/// <param name="s">socket of the sending client</param>
	/// <param name="">data the client sent as a string</param>
	void onInputRecieved(SOCKET s, std::string data);
};
