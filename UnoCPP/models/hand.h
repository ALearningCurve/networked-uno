#pragma once
#include "card.h"
#include "../exceptions/exception.h"
#include <vector>

// TODO along with deck.h, game_state.h, player.h replace shared_ptr<Card> with unique_ptr<Card>

/// <summary>
/// Represents a hand in a game of UNO
/// </summary>
class Hand {
	std::vector<std::shared_ptr<Card>> _cards;
public:
	/// <summary>
	/// Creates a new empty hand
	/// </summary>
	Hand() {}

	// TODO make the cards arg take a reference
	/// <summary>
	/// Makes a new hand prefilled with the cards in the provided vector
	/// </summary>
	/// <param name="cards">cards to insert into the hand</param>
	Hand(std::vector<std::shared_ptr<Card>> cards) { _cards.swap(cards); }

	/// <summary>
	/// Adds a card to the hand
	/// </summary>
	/// <param name="c">the card to add to the hand</param>
	void add_card(std::shared_ptr<Card> c);

	/// <summary>
	/// removes the card from the hand at the given position
	/// </summary>
	/// <param name="pos">index of the card to remove</param>
	/// <returns>the removed card</returns>
	std::shared_ptr<Card> remove_card(int pos);

	// TODO have return standard ptr (if peeking then they are not an owner)
	/// <summary>
	/// peeks the card in the hand
	/// </summary>
	/// <param name="pos">the index of the card in the hand to peek</param>
	/// <returns></returns>
	const std::shared_ptr<Card> peek_card(int pos);

	/// <summary>
	/// Returns a vector of all the cards in the hand
	/// </summary>
	/// <returns>vector of all the cards in the hand</returns>
	const std::vector<std::shared_ptr<Card>>& cards() const;

	/// <summary>
	/// the numbre of cards in the hand
	/// </summary>
	/// <returns>the count of the number of cards in the hand</returns>
	int get_number_cards() const;
};