#include "controller.h"

void TextController::startGame() {
	_view.output("[ ] Starting Game in Mode: Text");
	auto players = _model.get_players();
	auto turn_iter = players.begin();

	while (!_quit) {
		Player& player = _model.get_current_player();
		_view.output("");
		_view.output("[!] Its " + player.format() + "'s turn");
		outputGameState(player);
		playerDoTurn(player);
		_model.move_to_next_player();
	}

	_view.output("[x] Thanks for playing, quitting game!");
}

void TextController::outputGameState(const Player& currentPlayer)
{
	_view.output("Current Game State");
	for (const Player& player : _model.get_players()) {
		_view.output(player.get_name() + ": with " + std::to_string(player.get_hand().get_number_cards()) + " cards");
	}

	std::stringstream ss;
	ss << "Your hand";
	for (const Card& card : _model.get_current_player().get_hand().cards()) {
		ss << ", " << card.format();
	}
	_view.output(ss.str());
}

const void TextController::playerDoTurn(Player& player) {
	while (true) {
		// every time we are here we are looking for input
		_view.output("[ ] Enter Input ('help' for help)");

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
			_view.output("[X] Unknown Command: '" + command_name + "'");
		}
		else {
			std::vector<std::string> vec;
			split_str(uinput, " ", vec);
			vec.erase(vec.begin()); // get rid of the first input (which is the command name
			TextCommand* command;

			try {
				command = commandEntry->second(vec);
			}
			catch (const std::exception& ex) {
				_view.output(std::string("[X] BAD INPUT: ") + ex.what());
				continue;
			} 
			catch (...) {
				return _view.output("[X] BAD INPUT: failed to create command with given parameters");
				std::rethrow_exception(std::current_exception());
				continue;
			}

			try {
				command->run(this->_model, this->_view);
			}
			catch (const std::exception& ex) {
				_view.output(std::string("[X] ERROR EXECUTING COMMAND " + command->get_name() + ": ") + ex.what());
			}
			catch (...) {
				_view.output("[X] UNHANDLDED ERROR EXECUTING COMMAND " + command->get_name());
				std::rethrow_exception(std::current_exception());
			}
			delete command;
			return;
		}
	}
}

