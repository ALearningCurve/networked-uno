#include "controller.h"

void TextController::startGame() {
	_view.alert("Starting Game in Mode: Text");
	auto players = _model.get_players();
	auto turn_iter = players.begin();

	const Player* winner = _model.get_winner();
	while (!(_quit || winner)) {
		Player* player = _model.get_current_player();
		_view.info("Its " + _view.stringify_player(*player) + "'s turn");
		_view.info(_view.stringify_current_turn(_model));
		playerDoTurn();
		if (!(winner = _model.get_winner())) {
			_view.info(_view.stringify_player(*player) + "'s turn is over\n\n");
			_model.start_next_turn();
		}
	}
	if (winner) {
		_view.alert("Congrats " + _view.stringify_player(*winner) + ", you have won!");
	}

	_view.alert("Thanks for playing, quitting game!");
}

std::map<std::string, VecCommand> TextController::make_dict()
{
	std::map<std::string, VecCommand> m;
	m["play"] = [](auto str) { return std::make_shared<PlayCommand>(str); };
	m["draw"] = [](auto str) { return std::make_shared<DrawCommand>(); };
	m["uno"] = [](auto str) { return std::make_shared<UnoCommand>(); };
	m["help"] = [](auto str) { return std::make_shared<HelpCommand>(); };

	return m;
}

const void TextController::playerDoTurn() {
	while (true) {
		// every time we are here we are looking for input
		_view.alert("Enter Input ('help' for help)");

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
			_view.error("Unknown Command: '" + command_name + "'");
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
				_view.error(std::string("BAD INPUT: ") + ex.what());
				continue;
			} 
			catch (...) {
				return _view.error("BAD INPUT: failed to create command with given parameters");
				std::rethrow_exception(std::current_exception());
				continue;
			}

			try {
				command->run(this->_model, this->_view);
			}
			catch (const std::exception& ex) {
				_view.error(std::string("ERROR EXECUTING COMMAND " + command->get_name() + ": ") + ex.what());
				// if there was an error, let the player have the chance to retry
				continue;
			}
			catch (...) {
				_view.error("UNHANDLDED ERROR EXECUTING COMMAND " + command->get_name());
				std::rethrow_exception(std::current_exception());
			}

			if (command->takes_whole_turn()) {
				return;
			}
		}
	}
}

