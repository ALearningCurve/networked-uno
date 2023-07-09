#include "controller.h"

void TextController::startGame() {
	_view->alert("Starting Game in Mode: Text");
	auto players = _model.get_players();
	auto turn_iter = players.begin();

	const Player* winner = _model.get_winner();
	while (!(_quit || winner)) {
		Player* player = _model.get_current_player();
		_view->info("Its " + _view->stringify_player(*player) + "'s turn");
		_view->info(_view->stringify_current_turn(_model));
		playerDoTurn();
		if (!(winner = _model.get_winner())) {
			_view->info(_view->stringify_player(*player) + "'s turn is over\n\n");
			_model.start_next_turn();
		}
	}
	if (winner) {
		_view->alert("Congrats " + _view->stringify_player(*winner) + ", you have won!");
	}

	_view->alert("Thanks for playing, quitting game!");
}

std::map<std::string, UnoGameCommandFactory> TextController::make_dict()
{
	std::map<std::string, UnoGameCommandFactory> m;
	m["play"]		= [](auto args) { return std::make_shared<PlayCommand>(args); };
	m["draw"]		= [](auto args) { return std::make_shared<DrawCommand>(); };
	m["uno"]		= [](auto args) { return std::make_shared<UnoCommand>(); };
	m["help"]		= [](auto args) { return std::make_shared<HelpCommand>(); };
	m["play+uno"]	= [](auto args) { return std::make_shared<PlayThenUnoCommand>(args); };

	return m;
}

const void TextController::playerDoTurn() {
	while (true) {
		// every time we are here we are looking for input
		_view->alert("Enter Input ('help' for help)");

		if (_input.eof()) {
			throw Ex("Unexpectedly ran out of input...");
		}

		// Read input from the user
		std::string uinput;
		std::getline(_input, uinput);

		// if the command is quit, then we want to quit
		if (uinput == "q") {
			_quit = true;
			return;
		}
	
		auto maybeCommand = getCommandFromMap<UnoGameCommandFactory>(_command_dict, _view, uinput);
		if (!maybeCommand.has_value()) {
			continue;
		} else {
			auto [commandFactory, argsVec] = maybeCommand.value();
			auto command = commandFactory(argsVec);
			try {
				command ->run(this->_model, this->_view);
			}
			catch (const std::exception& ex) {
				_view->error(std::string("ERROR EXECUTING COMMAND " + command->get_name() + ": ") + ex.what());
				// if there was an error, let the player have the chance to retry
				continue;
			}
			catch (...) {
				_view->error("UNHANDLDED ERROR EXECUTING COMMAND " + command->get_name());
				std::rethrow_exception(std::current_exception());
			}

			if (command->takes_whole_turn()) {
				return;
			}
		}
	}
}

template <typename T>
std::optional<std::pair<T,std::vector<std::string>>> getCommandFromMap(std::map<std::string, T> commands, TextView* view, std::string& uinput)
{
	std::string command_name = lsplit_str(uinput);
	const auto& commandEntry = commands.find(command_name);

	if (commandEntry == commands.end()) {
		view->error("Unknown Command: '" + command_name + "'");
	}
	else {
		std::vector<std::string> vec;
		split_str(uinput, " ", vec);
		vec.erase(vec.begin()); // get rid of the first input (which is the command name
		std::shared_ptr<UnoGameTextCommand> command;

		try {
			return std::optional{ std::make_pair(commandEntry->second, vec) };
		}
		catch (const std::exception& ex) {
			view->error(std::string("BAD INPUT: ") + ex.what());
			return std::nullopt;
		}
		catch (...) {
			view->error("BAD INPUT: failed to create command with given parameters");	
			return std::nullopt;
		}
	}
}

void SimpleSocketBasedController::startGame() {
	std::cout << "[ ] Starting UNO networked server controller" << std::endl;
	server.start();
	server.spin(500);
}

void SimpleSocketBasedController::onDisconnect(SOCKET s)
{
	// remove from client map
	auto infoIter = clientMap.find(s);
	if (infoIter == clientMap.end()) {
		throw std::exception("Invalid state, unconnected socket disconnected from server");
	}

	if (infoIter->second != "") {
		auto& lobby = lobbyManager.getLobby(infoIter->second);
		for (auto& client : lobby.clients) {
			clientMap.erase(client->getSocket());
		}
		lobbyManager.removeLobby(infoIter->second);
	}

	clientMap.erase(infoIter);
}

void SimpleSocketBasedController::onClientConnected(SOCKET s)
{
	// enter client into a map with no lobby id ("")
	clientMap[s] = "";
}

void SimpleSocketBasedController::onInputRecieved(SOCKET s, std::string uinput)
{
	std::string fromController = "from controller";
	server.sendClientMessage(s, fromController);
	auto iter = clientMap.find(s);
	if (iter == clientMap.end()) {
		throw std::exception("Client was incorrectly dropped from client map, aborting program.");
	}
	SocketView requesterView({ s }, server);

	if (iter->second == "") {
		auto maybeCommand = getCommandFromMap<LobbyCommandFactory>(pregameCommands, &requesterView, uinput);
		if (!maybeCommand.has_value()) {
			requesterView.error("Enter a valid command name next time!");
		}
		else {
			auto [commandFactory, argsVec] = maybeCommand.value();
			auto command = commandFactory(argsVec);
			try {
				command->run(s, lobbyManager, &requesterView);
			}
			catch (const std::exception& ex) {
				requesterView.error(std::string("ERROR EXECUTING COMMAND " + command->get_name() + ": ") + ex.what());
				// if there was an error, let the player have the chance to retry
				return;
			}
			catch (...) {
				requesterView.error("UNHANDLDED ERROR EXECUTING COMMAND " + command->get_name());
				std::rethrow_exception(std::current_exception());
			}
		}
	}
	else {
		// handle in game: take turn like normal
		requesterView.error("Not yet implemented");
	}
	
}

std::map<std::string, LobbyCommandFactory> SimpleSocketBasedController::make_pregame_command_dict()
{
	std::map<std::string, LobbyCommandFactory> m;
	m["help"]  = [](auto args) { return std::make_shared<LobbyHelpCommand>(args); };
	/*m["new"]   = [](auto args) { return std::make_shared<NewLobbyCommand>(); };
	m["join"]  = [](auto args) { return std::make_shared<JoinLobbyCommand>(); };
	m["start"] = [](auto args) { return std::make_shared<StartLobbyCommand>(); };*/

	return m;
}

const std::map<std::string, LobbyCommandFactory> SimpleSocketBasedController::pregameCommands = SimpleSocketBasedController::make_pregame_command_dict();