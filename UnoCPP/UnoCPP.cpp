// UnoCPP.cpp : Defines the entry point for the application.
//
#include "UnoCPP.h"
#include "models/card.h"
using namespace std;


int main()
{
	Deck deck;
	Player p1("player 1");
	Player p2("player 2");
	Player p3("player 3");
	std::vector<Player> players = { p1, p2, p3 };
	GameState game(deck, players);

	cout << "-------------------------------" << endl;
	cout << "Starting UNO" << endl;
	cout << "-------------------------------\n\n" << endl;

	//std::map<std::st/*ring, std::shared_ptr<TextCommand>> m = make_dict();
	//std::shared_ptr<TextCommand> command = m.find("play")->second;
	//command->run(game);*/


	TextView view(cout);
	TextController controller(game, view, cin);
	controller.startGame();
	return 0;
}