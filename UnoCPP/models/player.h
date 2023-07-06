#pragma once
#include <cstring>
#include <sstream>
#include "card.h"
#include "hand.h"
#include "../exceptions/exception.h"
#include <WinSock2.h>

class Player {
	std::string _name;
	Hand _hand;
	bool _is_bot=false;
	bool _is_uno_immune=false;
public:
	Player(std::string name) : _name(name) {};
	//const Card& play_card(std::shared_ptr<Card> card);
	const std::shared_ptr<Card> play_card(int card);
	void add_card(std::shared_ptr<Card> card);

	Hand& get_hand();
	const Hand& get_hand() const;
	bool is_bot() const;
	const std::string& get_name() const;
	bool set_uno_immunity(bool val);
	bool is_uno_vulnerable() const;
};

class SocketPlayer : public Player {
	SOCKET _socket;
public:
	SocketPlayer(std::string name, SOCKET socket) : Player(name), _socket(socket) {};
	SOCKET getSocket() { return _socket;  }
};