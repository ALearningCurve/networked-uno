#pragma once
#include "../models/game_state.h"
#include "../models/player.h"
#include "../models/card.h"
#include "../view/text_view.h"
#include "../controller/tcp_server.h"
#include <iostream>
#include <istream>
#include <map>
#include "command.h"
#include <memory>
#include <functional>
#include <WinSock2.h>
#include <utility>

using UnoGameCommandFactory = std::function<std::shared_ptr<UnoGameTextCommand>(const std::vector<std::string>&)>;
using LobbyCommandFactory = std::function<std::shared_ptr<LobbyTextCommand>(const std::vector<std::string>&)>;


class Controller {
public:
	virtual ~Controller() = default;
	virtual void startGame() = 0;
};

// For local multiplayer
class TextController : Controller {
	GameState& _model;
	TextView* _view;
	std::istream& _input;
	bool _quit = false;
	const static std::map<std::string, UnoGameCommandFactory> _command_dict;
	const void playerDoTurn();
public:
	TextController(GameState& model, TextView* view, std::istream& istream) : _model(model), _view(view), _input(istream) {};
	static std::map<std::string, UnoGameCommandFactory> make_uno_game_commands();
	void startGame();
};

class SimpleSocketBasedController : public Controller, public ServerInterface {
	// invariant: if ClientInformation in client map has lobby_id, then
	// that lobby must exist in lobby map
	LobbyManager lobbyManager;
	TcpServer server;

	const static std::map<std::string, LobbyCommandFactory> _lobby_command_dict;
	const static std::map<std::string, UnoGameCommandFactory> _uno_game_command_dict;

public:
	static std::map<std::string, LobbyCommandFactory> make_lobby_command_dict();
	SimpleSocketBasedController() : server(this) {}

	/// <summary>
	/// Starts the networked server
	/// </summary>
	void startGame();

	/// <summary>
	/// handles the event of a client disconnecting from the server.
	/// If client disconnects and is in a lobby, it will end that lobby.
	/// </summary>
	/// <param name="s">socket of the disconnected client</param>
	void on_disconnect(SOCKET s);

	/// <summary>
	/// handles the event of a client connecting to the server
	/// </summary>
	/// <param name="s">socket of the conntected client</param>
	void on_client_connected(SOCKET s);

	/// <summary>
	/// handles the event of a client sending data to the server.
	/// Two unique cases:
	///		- If the client is not in a lobby, they must either start or join a lobby
	///		- If the client is in a lobby, they must play the game until completion
	/// </summary>
	/// <param name="s">socket of the sending client</param>
	/// <param name="">data the client sent as a string</param>
	void on_input_recieved(SOCKET s, std::string data);
};

namespace CommandUtils {
	template <typename T>
	std::optional<std::pair<T, std::vector<std::string>>> getCommandFromMap(std::map<std::string, T> commands, TextView* view, std::string& uinput);

	std::optional<std::shared_ptr<UnoGameTextCommand>> getAndRunUnoGameCommand(const std::map<std::string, UnoGameCommandFactory>& commands, std::string& uinput, TextView* userView, TextView* wholeGameView, GameState& game, Player* user);
};
