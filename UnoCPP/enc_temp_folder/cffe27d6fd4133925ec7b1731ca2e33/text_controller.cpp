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

void SimpleSocketBasedController::startGame() {
	std::cout << "[ ] Starting UNO networked server controller" << std::endl;
	server.start();
	server.spin(500);
}

void SimpleSocketBasedController::onDisconnect(SOCKET s)
{
	// remove from client map
	lobbyManager.removeClient(s);
}

void SimpleSocketBasedController::onClientConnected(SOCKET s)
{
	lobbyManager.addClient(s);
}

void SimpleSocketBasedController::onInputRecieved(SOCKET s, std::string uinput)
{
	std::string fromController = "from controller";
	server.sendClientMessage(s, fromController);
	auto clientLobbyId = lobbyManager.getClientLobbyId(s);
	std::vector<SOCKET> requesterVec;
	requesterVec.push_back(s);
	SocketView requesterView(requesterVec, server);

	if (!clientLobbyId.has_value()) {
		auto maybeCommand = getCommandFromMap<LobbyCommandFactory>(pregameCommands, &requesterView, uinput);
		if (!maybeCommand.has_value()) {
			return requesterView.error("Enter a valid command name next time!");
		}
		auto [commandFactory, argsVec] = maybeCommand.value();

		std::shared_ptr<LobbyTextCommand> command = nullptr;
		// try parsing the user input into a command that we can then run
		try {
			command = commandFactory(argsVec);
		}
		catch (const std::exception& ex) {
			return requesterView.error(std::string("BAD INPUT: ") + ex.what());
		}
		catch (...) {
			return requesterView.error("BAD INPUT: failed to create command with given parameters");
		}

		// now try running the command
		try {
			command->run(s, lobbyManager, &requesterView);
		}
		catch (const std::exception& ex) {
			return requesterView.error(std::string("ERROR EXECUTING COMMAND " + command->get_name() + ": ") + ex.what());
		}
		catch (...) {
			requesterView.error("UNHANDLDED ERROR EXECUTING COMMAND " + command->get_name());
			std::rethrow_exception(std::current_exception());
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
	m["new"]   = [](auto args) { return std::make_shared<NewLobbyCommand>(args); };
	m["join"]  = [](auto args) { return std::make_shared<JoinLobbyCommand>(args); };

	return m;
}

const std::map<std::string, LobbyCommandFactory> SimpleSocketBasedController::pregameCommands = SimpleSocketBasedController::make_pregame_command_dict();