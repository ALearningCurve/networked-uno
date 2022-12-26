#include "controller.h"

void TextController::startGame() {
	_view.output("--- Starting Game in Mode: Text");

	while (!_quit) {
		for (auto player : _model.get_players()) {
			_view.output("[x] Turn: " + player.format());
			playerGetTurn(player);
		}
		return;
	}

	_view.output("[x] Thanks for playing, quitting game!");
}




const Card& TextController::playerGetTurn(Player& player) {
	
	while (card_num == -100) {
		if (std::cin.eof()) {
			throw Ex("Unexpectedly ran out of input...");
		}

	    _input >> card_num;

		// if there is not a integer, then ask for more input
		if (_input.fail()) {
			_view.output("Bad input!");
			_input.clear();
			_input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

	}



	_input >> card_num;
	std::cout << "Hello " << card_num;
	
	return player.play_card(card_num);
}

