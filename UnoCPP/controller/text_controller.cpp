#include "controller.h"

void TextController::startGame() {
	_view.output("--- Starting Game in Mode: Text");

	while (!_quit) {
		for (auto player : _model.get_players()) {
			_view.output("[x] Turn: " + player.format());
			playerDoTurn(player);
		}
		return;
	}

	_view.output("[x] Thanks for playing, quitting game!");
}




const void TextController::playerDoTurn(Player& player) {
	
	while (true) {


		if (_input.eof()) {
			throw Ex("Unexpectedly ran out of input...");
		}

		// Read input from the user
		std::string uinput;
		_input >> uinput;

		// if the command is quit, then we want to quit
		if (uinput == "q") {
			_quit = true;
			return;
		}

		std::string command_name = lsplit_str(uinput);
		const auto iter = _command_dict.find(command_name);


		if (iter == _command_dict.end()) {
			// if the command is not recognized, then we want to 
			// let the user know and output it to the console

			_view.output("Unknown Command: '" + command_name + "'");
			
		}
		else {
			// if the command is recognized, then we want to let the 
			// user know
			_view.output("[ ] Running " + command_name);
			// and then we want to run the command
			// TODO: for now we want to ignore

			// try running
			// if error output message
			break;
		}
	}
}

