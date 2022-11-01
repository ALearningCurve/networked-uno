#pragma once
#include "../models/game_state.h"
#include "../models/player.h"
#include "../models/card.h"
#include "../view/text_view.h"



class Controller {
public:
	virtual void startGame() = 0;
};

class TextController : Controller {
	GameState _model;
	TextView _view;

public:
	TextController(GameState& model, TextView& view) : _model(model), _view(view) {};

	void startGame();
};