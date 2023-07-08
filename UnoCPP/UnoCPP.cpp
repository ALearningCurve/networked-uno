// UnoCPP.cpp : Defines the entry point for the application.
//
#include "UnoCPP.h"

int main()
{
	//Deck deck;
	//Player p1("player 1");
	//Player p2("player 2");
	////Player p3("player 3");
	//std::vector<Player*> players = { &p1, &p2 };
	//GameState game(deck, players);
	//StreamView view(std::cout);
	//TextView* tv = &view;
	//TextController controller(game, tv, std::cin);
	//controller.startGame();
	SimpleSocketBasedController foo;
	foo.startGame();
	return 0;
}