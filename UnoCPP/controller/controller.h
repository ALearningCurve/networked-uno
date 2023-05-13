#pragma once
#include "../models/game_state.h"
#include "../models/player.h"
#include "../models/card.h"
#include "../view/text_view.h"
#include <iostream>
#include <istream>
#include <map>
#include "command.h"
#include <functional>

using VecCommand = std::function<TextCommand*(const std::vector<std::string>&)>;

class Controller {
public:
	virtual void startGame() = 0;
};


class TextController : Controller {
	GameState& _model;
	TextView& _view;
	std::istream& _input;
	bool _quit = false;

	void outputGameState(const Player& currentPlayer);

	std::map<std::string, VecCommand> make_dict();

	const std::map<std::string, VecCommand> _command_dict = make_dict();

	const void playerDoTurn();

public:
	TextController(GameState& model, TextView& view, std::istream& istream) : _model(model), _view(view), _input(istream) {};

	void startGame();
};
