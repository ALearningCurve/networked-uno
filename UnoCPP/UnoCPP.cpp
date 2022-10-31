// UnoCPP.cpp : Defines the entry point for the application.
//
#include "UnoCPP.h"
#include "models/card.h"
using namespace std;

int main()
{
	cout << "Starting UNO" << endl;
	// Create a controller
	Card c("4", "green");
	cout << c.format() << endl;
	return 0;
}


// THINGS WE NEED:
//	- Card