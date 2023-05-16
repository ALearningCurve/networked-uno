#include "controller.h"

void TextController::startGame() {
	_view.output("[ ] Starting Game in Mode: Text");
	auto players = _model.get_players();
	auto turn_iter = players.begin();

	const Player* winner = _model.get_winner();
	while (!(_quit || winner)) {
		Player* player = _model.get_current_player();
		_view.output("");
		_view.output("[!] Its " + player->format() + "'s turn");
		outputGameState(*player);
		playerDoTurn();
		if (!(winner = _model.get_winner())) {
			_model.start_next_turn();
		}
	}
	if (winner) {
		_view.output("[!] Congrats " + winner->get_name() + ", you have won!");
	}

	_view.output("[x] Thanks for playing, quitting game!");
}

void TextController::outputGameState(const Player& currentPlayer)
{
	_view.output("[ ] Current Game State");
	_view.output("\tLast played card: " + _model.get_last_card()->format());
	std::stringstream ss;
	ss << "\tYour hand: ";
	for (const std::shared_ptr<Card> card : _model.get_current_player()->get_hand().cards()) {
		ss << card->format();
		if (&card != &_model.get_current_player()->get_hand().cards().back()) {
			ss << ", ";
		}
	}
	_view.output(ss.str());
	_view.output("\tPlayer info:");
	for (Player* player : _model.get_players()) {
		_view.output("\t\t" + player->get_name() + ": with " + std::to_string(player->get_hand().get_number_cards()) + " cards");
	}
}

std::map<std::string, VecCommand> TextController::make_dict()
{
	std::map<std::string, VecCommand> m;
	m["play"] = [](auto vec) { return new PlayCommand(vec); };
	m["draw"] = [](auto vec) {return new DrawCommand(); };
	m["uno"] = [](auto vec) {return new UnoCommand(); };
	m["help"] = [](auto vec) {return new HelpCommand(); };

	return m;
}

const void TextController::playerDoTurn() {
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
				// if there was an error, let the player have the chance to retry
				continue;
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

