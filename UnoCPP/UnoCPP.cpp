// UnoCPP.cpp : Defines the entry point for the application.
//
#include "UnoCPP.h"
#include "models/card.h"
using namespace std;

int main()
{
	cout << "-------------------------------" << endl;
	cout << "Starting UNO" << endl;
	cout << "-------------------------------\n\n\n" << endl;


	Deck deck;
	Player p1("player 1");
	Player p2("player 2");
	Player p3("player 3");
	std::vector<Player> players = { p1, p2, p3 };
	GameState game(deck, players);
	TextView view;
	TextController controller(game, view);

	return 0;
}


// THINGS WE NEED:
//	- Card