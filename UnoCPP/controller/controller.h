#pragma once
#include "../models/game_state.h"
#include "../models/player.h"
#include "../models/card.h"
#include "../view/text_view.h"
#include <iostream>
#include <istream>
#include <map>
#include "command.h"

class Controller {
public:
	virtual void startGame() = 0;
};


class TextController : Controller {
	GameState& _model;
	TextView& _view;
	std::istream& _input;
	const Card& playerGetTurn(Player& player);
	bool _quit = false;
	const static std::map<std::string, TextCommand> _command_dict;

public:
	TextController(GameState& model, TextView& view, std::istream& istream) : _model(model), _view(view), _input(istream) {};

	void startGame();
};

const std::map<std::string, TextCommand> TextController::_command_dict = {
	{"play", nullptr},
	{"draw", nullptr},
	{"uno", nullptr}
};