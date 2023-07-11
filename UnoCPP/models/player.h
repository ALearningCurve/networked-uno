#pragma once
#include <cstring>
#include <sstream>
#include "card.h"
#include "hand.h"
#include "../exceptions/exception.h"


// TODO along with deck.h, game_state.h, hand.h replace shared_ptr<Card> with unique_ptr<Card>


/// <summary>
/// Represents a player in the game of UNO
/// </summary>
class Player {
	std::string _name;
	Hand _hand;
	bool _is_bot=false;
	bool _is_uno_immune=false;
public:
	/// <summary>
	/// Creates a new player with the given name
	/// </summary>
	/// <param name="name">the name of the player to create</param>
	Player(std::string name) : _name(name) {};

	/// <summary>
	/// plays the given card index and removes it from this player's hand
	/// </summary>
	/// <param name="card">the index of the card to play</param>
	/// <returns>the card that was played</returns>
	const std::shared_ptr<Card> play_card(int card);

	/// <summary>
	/// adds the given card to the player's hand
	/// </summary>
	/// <param name="card">the card to add</param>
	void add_card(std::shared_ptr<Card> card);

	/// <summary>
	/// gets this player's hand
	/// </summary>
	/// <returns>this player's hand</returns>
	Hand& get_hand();

	/// <summary>
	/// get this player's hand
	/// </summary>
	/// <returns>this player's hand</returns>
	const Hand& get_hand() const;

	/// <summary>
	/// returns true if this player is a bot
	/// </summary>
	/// <returns></returns>
	bool is_bot() const;

	/// <summary>
	/// gets this player's name
	/// </summary>
	/// <returns>the name of this player</returns>
	const std::string& get_name() const;

	/// <summary>
	/// sets the uno immmunity for this player
	/// </summary>
	/// <param name="val">true if they should be immune. cannot be true if player does not have 1 card.</param>
	/// <returns>whether this player is uno immune</returns>
	bool set_uno_immunity(bool val);

	/// <summary>
	/// whether or not this player is uno vulnerable
	/// </summary>
	/// <returns>true if this player is uno vulnerable</returns>
	bool is_uno_vulnerable() const;
};