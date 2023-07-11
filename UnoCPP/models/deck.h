#pragma once

#include "card.h"
#include "hand.h"
#include <algorithm>
#include <random>
#include <chrono>
#include "../exceptions/exception.h"

/// <summary>
/// Represents a deck of cards that is used in an uno game
/// </summary>
class Deck
{
	// TODO change from shared_ptr to unique_ptr
	// Note: We use an actual array of cards (instead of on the fly generation)
	// because I personally really like how when you 
	// play uno you have to reshuffle periodically,
	// so with this array a future implementation can show the 
	// # in the draw/discard piles
	std::vector<std::shared_ptr<Card>> _cards;

	/// <summary>
	/// makes a deck of cards and adds it to the _cards
	/// </summary>
	void create_card_deck();
public:
	/// <summary>
	/// Creates a deck of uno cards
	/// </summary>
	Deck(): Deck(2) {};

	/// <summary>
	/// Creates a deck with n amount of complete uno card decks
	/// </summary>
	/// <param name="n">number of decks to create</param>
	Deck(int n) { create_n_card_decks(n); shuffle_deck(); };

	/// <summary>
	/// Shuffles this card deck
	/// </summary>
	void shuffle_deck();

	/// <summary>
	/// Creates a deck with n amount of complete uno card decks and adds it to this deck's
	/// cards at the bottom of the deck
	/// </summary>
	/// <param name="n">number of decks to create</param>
	void create_n_card_decks(int n);

	/// <summary>
	/// gets the number of cards in the deck
	/// </summary>
	/// <returns>the integer count of the number of cards in the deck</returns>
	int get_card_count();

	/// <summary>
	/// Draws a card from the deck
	/// </summary>
	/// <returns>the card that was drawn</returns>
	std::shared_ptr<Card> draw_card();

	/// <summary>
	/// Deals the given number of cards to the provided hand
	/// </summary>
	/// <param name="h">the hand to deal to </param>
	/// <param name="n">the number of cards to deal</param>
	void deal_hand(Hand& h, int n);

	/// <summary>
	/// returns the deck
	/// </summary>
	/// <returns>vector with all the cards</returns>
	const std::vector<std::shared_ptr<Card>>& get_deck();
};