#include "Deck.h"
#include "Stack.h"
#include "Game.h"
// Default ctor
Deck::Deck()
{
	/* TODO Lab3:
			Implement this method.

			Get everything set up by calling Initialize.
	*/
	Initialize();
}

// Sets up the array of cards to be a unique deck
void Deck::Initialize()
{
	/* TODO Lab3:
			Implement this method.
	
			Set the values in the m_cards array to be the appropriate values for your game.

			For Go Fish: 13 cards of each of the 4 suits

			For UNO: 19 (B)lue cards (one 0, two each 1-9)
					 19 (G)reen cards (one 0, two each 1-9)
					 19 (R)ed cards (one 0, two each 1-9)
					 19 (Y)ellow cards (one 0, two each 1-9)
					  8 Draw 2 cards (two in each color) - Face value 10
					  8 Reverse cards (two in each color) - Face value 11
					  8 Skip cards (two in each color) - Face value 12
					  4 Wild cards (even though these are "colorless," making one each color is easier) - Face value 13
					  4 Wild Draw 4 cards (even though these are "colorless," making one in each color is easier) - Face value 14
	*/

	int numCards = 0;

	for (int i = 'B'; i <= 'B'; i++)
	{
		for (int y = 0; y < 15; y++)
		{
			m_cards[numCards].SetFace(y);
			m_cards[numCards].SetSuit(i);
			++numCards;
		}
	}

	for (int i = 'B'; i <= 'B'; i++)
	{
		for (int x = 1; x < 13; x++)
		{
			m_cards[numCards].SetFace(x);
			m_cards[numCards].SetSuit(i);
			++numCards;
		}
	}

	for (int i = 'Y'; i <= 'Y'; i++)
	{
		for (int y = 0; y < 15; y++)
		{
			m_cards[numCards].SetFace(y);
			m_cards[numCards].SetSuit(i);
			++numCards;
		}
	}

	for (int i = 'Y'; i <= 'Y'; i++)
	{
		for (int x = 1; x < 13; x++)
		{
			m_cards[numCards].SetFace(x);
			m_cards[numCards].SetSuit(i);
			++numCards;
		}
	}

	for (int i = 'R'; i <= 'R'; i++)
	{
		for (int y = 0; y < 15; y++)
		{
			m_cards[numCards].SetFace(y);
			m_cards[numCards].SetSuit(i);
			++numCards;
		}
	}

	for (int i = 'R'; i <= 'R'; i++)
	{
		for (int x = 1; x < 13; x++)
		{
			m_cards[numCards].SetFace(x);
			m_cards[numCards].SetSuit(i);
			++numCards;
		}
	}

	for (int i = 'G'; i <= 'G'; i++)
	{
		for (int y = 0; y < 15; y++)
		{
			m_cards[numCards].SetFace(y);
			m_cards[numCards].SetSuit(i);
			++numCards;
		}
	}

	for (int i = 'G'; i <= 'G'; i++)
	{
		for (int x = 1; x < 13; x++)
		{
			m_cards[numCards].SetFace(x);
			m_cards[numCards].SetSuit(i);
			++numCards;
		}
	}
}

// Clears any cards that are already in the stack, shuffles the array, and then pushes
// all of the shuffled cards onto the stack.
void Deck::Shuffle()
{
	/* TODO Lab3:
			Implement this method.
	*/
	m_Stack.Clear();

	for (int s = 0; s < 400; s++)
		swap(m_cards[rand() % 108], m_cards[rand() % 108]);
	
	for (int p = 0; p < 108; p++)
		m_Stack.Push(m_cards[p]);
	
}

// Draw the top-most card off of the stack
// In:	_card			A reference to store the drawn card
//
// Return: True if a card was removed
//		   False if the deck was empty
bool Deck::Draw(Card& _card)
{
	/* TODO Lab3:
			Implement this method.
	*/
	if (m_Stack.Pop(_card))
		return true;
	else
		return false;

	
	// Just here for compilation
	return true;
}

// Clear the stack for re-use
void Deck::Clear()
{
	/* TODO Lab3:
			Implement this method.
	*/
	m_Stack.Clear();
}

// Check to see if there are any cards left in the stack
//
// Return: True if the stack is empty
//		   False if there are any cards left in the stack
bool Deck::IsEmpty() const
{
	/* TODO Lab3:
			Implement this method.
	*/

	int empty = m_Stack.GetCurrSize();

	if (0 == empty)
		return true;
	else
		return false;

	// Just here for compilation
	return true;
}

void Deck::AddCardToDeck(Card& _Deck)
{
	m_Stack.Push(_Deck);
}
