#pragma once
#include "stdafx.h"
#include "Card.h"
#include "Stack.h"
// Will need to include Stack.h when you get to that lab

// A Deck of cards
class Deck
{
	// Needed for unit tests
	// DO NOT REMOVE OR RENAME
#if GO_FISH
	Card m_cards[52];
#elif UNO
	Card m_cards[108];
#endif
	
	/* TODO Lab5:
			Update the Stack class to be a templated class.

			Update the m_Stack data member to be an object of a template class (that will store Card objects).
	*/

	// Uncomment this when you get the Stack class
	Stack<Card> m_Stack;

public:

	// Default ctor
	Deck();

	// Sets up the array of cards to be a unique deck
	void Initialize();

	// Clears any cards that are already in the stack, shuffles the array, and then pushes
	// all of the shuffled cards onto the stack.
	void Shuffle();

	// Draw the top-most card off of the stack
	// In:	_card			A reference to store the drawn card
	//
	// Return: True if a card was removed
	//		   False if the deck was empty
	bool Draw(Card& _card);

	// Clear the stack for re-use
	void Clear();

	// Check to see if there are any cards left in the stack
	//
	// Return: True if the stack is empty
	//		   False if there are any cards left in the stack
	bool IsEmpty() const;

	void Reshuffle();

	void AddCardToDeck(Card& _Deck);
	// Needed for unit tests
	// DO NOT REMOVE
	friend class CTestManager;

};

