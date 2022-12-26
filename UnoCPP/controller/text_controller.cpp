#include "controller.h"

void TextController::startGame() {
	_view.output("[ ] Starting Game in Mode: Text");
	auto players = _model.get_players();
	auto turn_iter = players.begin();

	while (!_quit) {
		Player player = *turn_iter;
		_view.output("");
		_view.output("[!] Its " + player.format() + "'s turn");
		playerDoTurn(player);
		
		
		if (_model.get_is_reversed()) {
			// if reversed, then it is previous person's turn
			if (turn_iter == players.begin()) {
				turn_iter = players.end();
			}
			--turn_iter;
		}
		else {
			// otherwise, it is the next person's turn
			++turn_iter;
			if (turn_iter == players.end()) {
				turn_iter = players.begin();
			}
		}
	}

	_view.output("[x] Thanks for playing, quitting game!");
}




const void TextController::playerDoTurn(Player& player) {
	
	while (true) {
		// every time we are here we are looking for input
		_view.output("[ ] Enter Input ('h' for help)");

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
		else if (uinput == "h") {
			_view.output(
				"Help Menu: \n"
				"  h: help \n"
				"  draw: draw a card\n"
				"  play: play a card\n"
				"  uno: shout uno\n"
				"  q: quit"
				);
		}

		std::string command_name = lsplit_str(uinput);
		const auto iter = _command_dict.find(command_name);


		if (iter == _command_dict.end()) {
			// if the command is not recognized, then we want to 
			// let the user know and output it to the console

			_view.output("[ ] Unknown Command: '" + command_name + "'");
			
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

