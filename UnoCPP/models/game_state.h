#pragma once

#include "deck.h"
#include "player.h"
#include <string>
#include "card.h"
#include <optional>

/// <summary>
/// Represents a whole uno game!
/// </summary>
class GameState {
	// TODO switch to unique_ptr
	std::vector<Player*> _players;
	Deck _draw_deck;
	Deck _discard_deck;

	std::shared_ptr<Card> _last_card;
	int _current_turn = 0;
	bool _is_reversed = false;
	int _draw_penalty = 0;
	bool _next_is_skippped = false;

	GameState();
	std::shared_ptr<Card> draw_card();
public:
	/// <summary>
	/// Creates a new game 
	/// </summary>
	/// <param name="deck">the deck to use as the draw deck</param>
	/// <param name="players">the players in this game</param>
	GameState(Deck deck, std::vector<Player*> players);

	/// <summary>
	/// gets the ptr of the current player
	/// </summary>
	/// <returns>Player ptr</returns>
	Player* get_current_player();

	/// <summary>
	/// Gets a vector of all the players in the game
	/// </summary>
	/// <returns>vector of all the players in the game</returns>
	const std::vector<Player*>& get_players();

	/// <summary>
	/// Ends the turn of the current player, applies any skips/draws, and 
	/// starts the turn for the player whose turn it is next
	/// </summary>
	void start_next_turn();

	/// <summary>
	/// Gets the player after the current player. Does not account for skips/draws, only
	/// the turn order and turn direction (which depends on if a reverse was played or not)
	/// </summary>
	/// <returns>index of the player who is the next player after the current player</returns>
	int get_next_player() const;

	/// <summary>
	/// Returns whether or not the game is reversed
	/// </summary>
	/// <returns>true if reversed</returns>
	bool get_is_reversed() const;

	/// <summary>
	/// Flips the turn order. Same as playing a reverse card.
	/// </summary>
	void flip_direction();

	// TODO return optional instread of nullptr
	/// <summary>
	/// Gets the winner player (if any)
	/// </summary>
	/// <returns>If winner returns a ptr to the winner, else returns a nullptr</returns>
	const Player* get_winner() const;

	/// <summary>
	/// Gets the last played card
	/// </summary>
	/// <returns>shared ptr to the last played card</returns>
	const std::shared_ptr<Card> get_last_card() const;

	/// <summary>
	/// Draws from the deck for the given player (must be their turn)
	/// </summary>
	/// <param name="player">player to draw for</param>
	/// <returns>the card they drew</returns>
	const std::shared_ptr<Card> draw_for_player(Player* player);

	/// <summary>
	/// plays the given card for the player
	/// </summary>
	/// <param name="player">the player to play for (must be their turn)</param>
	/// <param name="card">index of card in their hand</param>
	/// <param name="optWildColor">color for wild card to be (can only be not nullopt if card at index is wild)</param>
	/// <returns>Card that was played</returns>
	const Card& play_for_player(Player* player, const int& card, std::optional<std::string> optWildColor);
	
	/// <summary>
	/// Whether or not the player can play
	/// </summary>
	/// <param name="player">player who would play</param>
	/// <param name="card">the card index they would play</param>
	/// <param name="optWildColor">color for wild card to be (can only be not nullopt if card at index is wild)</param>
	/// <returns>std::nullopt if the player can play, but if they cannot will return a string with reason why not</returns>
	const std::optional<std::string> can_play(Player* player, const int& card, std::optional<std::string> optWildColor);
	
	/// <summary>
	/// Assumes that the given player shouted "UNO!"
	/// - if given player has one card they become uno immune
	/// - if some other player has one card, they get penalized and forced to draw cards
	/// </summary>
	/// <param name="player">the player ptr who shouted uno</param>
	void player_said_uno(Player* player);
};