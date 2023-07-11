// UnoCPP.cpp : Defines the entry point for the application.
//
#include "UnoCPP.h"

int main(int argc, char* argv[])
{

	if (argc == 2) {
		if (strcmp(argv[1], "local") == 0) {
			Deck deck;
			Player p1("player 1");
			Player p2("player 2");
			std::vector<Player*> players = { &p1, &p2 };
			GameState game(deck, players);
			StreamView view(std::cout);
			TextView* tv = &view;
			TextController controller(game, tv, std::cin);
			controller.start();
			return 0;
		} else if (strcmp(argv[1], "server") == 0) {
			SimpleSocketBasedController networkedController;
			networkedController.start();
			return 0;
		}
	}
	std::cout << "must specify one of two modes either " 
		<< "\"local\" (for local play on one terminal for 2 players) or "
		<< "\"server\" to start a hosted game server" << std::endl;
	return 1;


}