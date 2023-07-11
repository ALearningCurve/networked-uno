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

/// <summary>
/// Interface for uno game controller
/// </summary>
class Controller {
public:
	virtual ~Controller() = default;
	virtual void start() = 0;
};

/// <summary>
/// Controller for local multiplayer. Runs a single Uno Game which can be interacted
/// with via a text input stream.
/// </summary>
class TextController : Controller {
	GameState& _model;
	TextView* _view;
	std::istream& _input;
	bool _quit = false;
	const static std::map<std::string, UnoGameCommandFactory> _command_dict;
	const void playerDoTurn();
public:
	/// <summary>
	/// Constructor that makes a new constroller with the given model (holds game state),
	/// view (used to output game events to players), and istream (used to gather input from the users)
	/// </summary>
	TextController(GameState& model, TextView* view, std::istream& istream) : _model(model), _view(view), _input(istream) {};
	/// <summary>
	/// Makes a dictionary of the Uno Game Commands that can be used in this game
	/// </summary>
	static std::map<std::string, UnoGameCommandFactory> make_uno_game_commands();
	/// <summary>
	/// Starts the game using the model, view, and input stream specified in the constructor. 
	/// This function will block until the game is over.
	/// </summary>
	void start();
};

/// <summary>
/// A more complex, async version of the TextController that uses a TCP Server to 
/// handle networked clients. Supports the running of multiple games at the same time via the idea of lobbys!
/// </summary>
class SimpleSocketBasedController : public Controller, public ServerInterface {
	LobbyManager _lobby_manager;
	TcpServer _server;

	const static std::map<std::string, LobbyCommandFactory> _lobby_command_dict;
	const static std::map<std::string, UnoGameCommandFactory> _uno_game_command_dict;

public:
	static std::map<std::string, LobbyCommandFactory> make_lobby_command_dict();

	/// <summary>
	/// Initializes this controller and the TCP Server that is using. The server is not 
	/// started until "this->start()" is called
	/// </summary>
	SimpleSocketBasedController() : _server(this) {}

	/// <summary>
	/// Starts the networked server. This initializes the lobby manager as well
	/// to automatically handle clients connecting/disconnecting as well as joining
	/// different games. This function call blocks until the server is shut down.
	/// </summary>
	void start();

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
