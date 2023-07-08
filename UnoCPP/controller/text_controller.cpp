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

std::map<std::string, VecCommand> TextController::make_dict()
{
	std::map<std::string, VecCommand> m;
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

		std::string command_name = lsplit_str(uinput);
		const auto commandEntry = _command_dict.find(command_name);

		if (commandEntry == _command_dict.end()) {
			_view->error("Unknown Command: '" + command_name + "'");
		}
		else {
			std::vector<std::string> vec;
			split_str(uinput, " ", vec);
			vec.erase(vec.begin()); // get rid of the first input (which is the command name
			std::shared_ptr<TextCommand> command;

			try {
				command = commandEntry->second(vec);
			}
			catch (const std::exception& ex) {
				_view->error(std::string("BAD INPUT: ") + ex.what());
				continue;
			} 
			catch (...) {
				return _view->error("BAD INPUT: failed to create command with given parameters");
				std::rethrow_exception(std::current_exception());
				continue;
			}

			try {
				command->run(this->_model, this->_view);
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
		auto lobbyIter = lobbyMap.find(infoIter->second);
		if (lobbyIter == lobbyMap.end()) {
			throw std::exception("Invalid state, could not find lobby that should exist");
		}
		auto& clients = lobbyIter->second->clients;
		for (auto& client : clients) {
			clientMap.erase(client.getSocket());
		}
		lobbyMap.erase(lobbyIter);
	}

	clientMap.erase(infoIter);

}

void SimpleSocketBasedController::onClientConnected(SOCKET s)
{
	// enter client into a map with no lobby id ("")
	clientMap[s] = "";
}

void SimpleSocketBasedController::onInputRecieved(SOCKET s, std::string)
{
	std::string fromController = "from controller";
	server.sendClientMessage(s, fromController);
	auto iter = clientMap.find(s);
	if (iter == clientMap.end()) {
		throw std::exception("Client was incorrectly dropped from client map, aborting program.");
	}
	else {
		if (iter->second == "") {
			// handle not in lobby
			// - command: help
			// - command: new
			// - command: join
			// - command: start
		}
		else {
			// handle in game: take turn like normal
		}
	}
}

std::map<std::string, VecCommand> SimpleSocketBasedController::make_pregame_command_dict()
{
	std::map<std::string, VecCommand> m;
	m["help"]  = [](auto args) { return std::make_shared<PregameHelpCommand>(args); };
	m["new"]   = [](auto args) { return std::make_shared<PregameNewLobbyCommand>(); };
	m["join"]  = [](auto args) { return std::make_shared<PregameJoinLobbyCommand>(); };
	m["start"] = [](auto args) { return std::make_shared<PregameHelpCommand>(); };

	return m;
}

const std::map<std::string, VecCommand> SimpleSocketBasedController::pregameCommands = SimpleSocketBasedController::make_pregame_command_dict();