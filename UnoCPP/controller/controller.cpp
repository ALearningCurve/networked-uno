#include "controller.h"

void TextController::start() {
	_view->alert("Starting Game in Mode: Text");
	auto players = _model.get_players();
	auto turn_iter = players.begin();

	const Player* winner = _model.get_winner();
	while (!(_quit || winner)) {
		Player* player = _model.get_current_player();
		_view->info("Its " + _view->stringify_player(*player) + "'s turn");
		_view->info(_view->stringify_game_start_for_player(_model, _model.get_current_player()));
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

std::map<std::string, UnoGameCommandFactory> TextController::make_uno_game_commands()
{
	std::map<std::string, UnoGameCommandFactory> m;
	m["play"]		= [](auto args) { return std::make_shared<PlayCommand>(args); };
	m["draw"]		= [](auto args) { return std::make_shared<DrawCommand>(); };
	m["uno"]		= [](auto args) { return std::make_shared<UnoCommand>(); };
	m["help"]		= [](auto args) { return std::make_shared<HelpCommand>(); };
	m["gameinfo"]   = [](auto args) { return std::make_shared<ShowGameInfoCommand>(); };
	m["play+uno"]	= [](auto args) { return std::make_shared<PlayThenUnoCommand>(args); };

	return m;
}
const std::map<std::string, UnoGameCommandFactory> TextController::_command_dict = TextController::make_uno_game_commands();

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
	
		auto optCommand = CommandUtils::getAndRunUnoGameCommand(_command_dict, uinput, _view, _view, _model, _model.get_current_player());

		if (optCommand.has_value() && optCommand.value()->takes_whole_turn()) {
			return;
		}
	}
}

void SimpleSocketBasedController::start() {
	std::cout << "[ ] Starting UNO networked server controller" << std::endl;
	_server.start();
	_server.spin(500);
}

void SimpleSocketBasedController::on_disconnect(SOCKET s)
{
	// remove from client map
	auto optLobbyId = _lobby_manager.get_client_lobby_id(s);
	if (optLobbyId.has_value()) {
		auto& lobby = _lobby_manager.get_lobby(optLobbyId.value());
		std::vector<SOCKET> vec;
		for (auto& client : lobby._clients) {
			SOCKET socket = client.socket;
			if (socket != s) {
				vec.push_back(socket);
			}
		}
		SocketView view(vec, _server);
		view.alert("One of the memebers in the lobby disconnected, destroying lobby! You have been sent back to the lobby selction screen.");
	}
	_lobby_manager.remove_client(s);
}

void SimpleSocketBasedController::on_client_connected(SOCKET s)
{
	_lobby_manager.add_client(s);
	std::vector<SOCKET> vec = { s };
	SocketView view(vec, _server);
	view.alert("Welcome to the uno game server! You are in the lobby selection stage of the server. Type \"help\" for help.");
}

void SimpleSocketBasedController::on_input_recieved(SOCKET s, std::string uinput)
{
	auto clientLobbyId = _lobby_manager.get_client_lobby_id(s);
	std::vector<SOCKET> requesterVec = { s };
	SocketView requesterView(requesterVec, _server);
	// because the lobby of the client can change during command execution, 
	// we must use a dynamic lobby view
	DynamicClientLobbyView lobbyView(_lobby_manager, _server, s);

	if (!clientLobbyId.has_value()) {
		auto maybeCommand = CommandUtils::getCommandFromMap<LobbyCommandFactory>(_lobby_command_dict, &requesterView, uinput);
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
			command->run(s, _lobby_manager, &requesterView, &lobbyView);
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
		auto& lobby = _lobby_manager.get_lobby(clientLobbyId.value());
		if (!lobby._started) {
			return requesterView.error("Lobby has not yet started, your command was not evaluated. Sit tight until others join!");
		}

		if (lobby._game->get_winner() != nullptr) {
			// in theory this if is not possible if our code executes as we expect (since if a command errors 
			// then it should not modify the game state). Yet if there is some bug, where the game ends after
			// a command errors, add a check here to prevent undefined behavior with the game.
			lobbyView.alert("Game has ended, disconnect from the server to play another game!");
			_lobby_manager.remove_lobby(lobby._lobby_id);
		}

		auto optCommand = CommandUtils::getAndRunUnoGameCommand(_uno_game_command_dict, uinput, &requesterView, &lobbyView, *lobby._game, lobby.get_player_from_socket(s));

		// check if the game has ended
		const Player* winner;
		if ((winner = lobby._game->get_winner()) != nullptr) {
			lobbyView.alert("Congrats to " + lobbyView.stringify_player(*winner) + ", they have won! Ending game and closing lobby!");
			return _lobby_manager.remove_lobby(lobby._lobby_id);
		}
		
		if (optCommand.has_value() && optCommand.value()->takes_whole_turn()) {
			std::stringstream ss;
			ss << lobby._game->get_current_player()->get_name() << "'s turn has ended. Its ";
			lobby._game->start_next_turn();
			auto* currPlayer = lobby._game->get_current_player();
			ss << currPlayer->get_name() << "'s turn now.";
			lobbyView.alert(ss.str());

			SOCKET currPlayerSocket = INVALID_SOCKET;
			for (auto& client : lobby._clients) {
				if (currPlayer == client.player.get()) {
					currPlayerSocket = client.socket;
				}
			}
			if (currPlayerSocket == INVALID_SOCKET) {
				throw std::exception("Could not find next player's socket information");
			}
			std::vector<SOCKET> viewSockets = { currPlayerSocket };
			SocketView nextPlayerView(viewSockets, _server);
			ShowGameInfoCommand().run(*lobby._game, &nextPlayerView, &lobbyView, currPlayer);
		}
	}
}

std::map<std::string, LobbyCommandFactory> SimpleSocketBasedController::make_lobby_command_dict()
{
	std::map<std::string, LobbyCommandFactory> m;
	m["help"]  = [](auto args) { return std::make_shared<LobbyHelpCommand>(args); };
	m["new"]   = [](auto args) { return std::make_shared<NewLobbyCommand>(args); };
	m["join"]  = [](auto args) { return std::make_shared<JoinLobbyCommand>(args); };

	return m;
}

const std::map<std::string, LobbyCommandFactory> SimpleSocketBasedController::_lobby_command_dict = SimpleSocketBasedController::make_lobby_command_dict();
const std::map<std::string, UnoGameCommandFactory> SimpleSocketBasedController::_uno_game_command_dict = TextController::make_uno_game_commands();


template <typename T>
std::optional<std::pair<T, std::vector<std::string>>> CommandUtils::getCommandFromMap(std::map<std::string, T> commands, TextView* view, std::string& uinput)
{
	std::string command_name = lsplit_str(uinput);
	const auto& commandEntry = commands.find(command_name);

	if (commandEntry == commands.end()) {
		view->error("Unknown Command: '" + command_name + "'");
		return std::nullopt;
	}
	std::vector<std::string> vec;
	split_str(uinput, " ", vec);
	vec.erase(vec.begin()); // get rid of the first input (which is the command name
	std::shared_ptr<UnoGameTextCommand> command;
	return std::optional{ std::make_pair(commandEntry->second, vec) };
}

std::optional<std::shared_ptr<UnoGameTextCommand>> CommandUtils::getAndRunUnoGameCommand(const std::map<std::string, UnoGameCommandFactory>& commands, std::string& uinput, TextView* userView, TextView* wholeGameView, GameState& game, Player* user) {
	auto maybeCommand = getCommandFromMap<UnoGameCommandFactory>(commands, userView, uinput);
	std::optional<std::shared_ptr<UnoGameTextCommand>> command = std::nullopt;
	if (!maybeCommand.has_value()) {
		return std::nullopt;
	}
	else {
		auto [commandFactory, argsVec] = maybeCommand.value();
		try {
			command = commandFactory(argsVec);
		}
		catch (const std::exception& ex) {
			userView->error(std::string("BAD INPUT: ") + ex.what());
			return std::nullopt;
		}
		catch (...) {
			userView->error("BAD INPUT: failed to create command with given parameters");
			return std::nullopt;
		}
		try {
			command.value()->run(game, userView, wholeGameView, user);
		}
		catch (const std::exception& ex) {
			userView->error(std::string("ERROR EXECUTING COMMAND " + command.value()->get_name() + ": ") + ex.what());
			// if there was an error, let the player have the chance to retry
			return std::nullopt;
		}
		catch (...) {
			userView->error("UNHANDLDED ERROR EXECUTING COMMAND " + command.value()->get_name());
			std::rethrow_exception(std::current_exception());
			return std::nullopt;
		}
	}
	return command;
}