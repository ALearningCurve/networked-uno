#pragma once
#include "../models/game_state.h"
#include "../models/player.h"
#include "../models/card.h"
#include "../models/.h"


class Controller {
public:
	virtual void startGame();
};

class TextController : Controller {
public:
	TextController(GameState& model, TextView& view) 
};