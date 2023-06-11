// UnoCPP.cpp : Defines the entry point for the application.
//
#include "UnoCPP.h"

int main()
{
	Deck deck;
	Player p1("player 1");
	Player p2("player 2");
	//Player p3("player 3");
	std::vector<Player*> players = { &p1, &p2 };
	GameState game(deck, players);
	TextView view(std::cout);
	TextController controller(game, view, std::cin);
	controller.startGame();
	return 0;
}