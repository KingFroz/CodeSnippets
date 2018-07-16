#pragma once
#include "stdafx.h"


class Card
{
	// Needed for unit tests
	// DO NOT REMOVE OR RENAME
	int m_face;		// 2-14, with 14 being the Ace
	char m_suit;	// 3-6 (3 = Hearts, 4 = Diamonds, 5 = Clubs, 6 = Spades)

public:

	// Default constructor
	// In:	_face			The face value of the card
	//		_suit			The suit of the card
	
	/* TODO Lab1:
			Make the default values for a card the 2 of hearts
	*/
	Card(int _face = 0, char _suit = 'B');

	// Destructor
	~Card();

	/* Accessors */

	/* TODO:
			Define the two "Get" methods.  These should return the 
			corresponding values from the invoking object.

			These can be inlined if desired.
	*/
	int GetFace() const { return m_face; };
	char GetSuit() const { return m_suit; };

	/* Mutators */

	/* TODO Lab1:
			Define the two "Set" methods.  These should set the
			corresponding value of the invoking object to the 
			value that is being passed in.

			These can be inlined if desired.
	*/
	void SetFace(int _face) { m_face = _face; };
	void SetSuit(char _suit) { m_suit = _suit; };

	// Display a card to the screen
	void Show() const;

	// Relational operators
	
	/* TODO Lab1:
			Prototype and define the six relational operators to compare cards.

			These methods will only be comparing the face of the cards, not the face and the suit.

			These can be inlined if desired.
	*/

#define CARD_REL_OP(ro)\
	bool operator ro(const Card& _rhs) const\
		{\
		return (m_face ro _rhs.m_face);\
		}

	CARD_REL_OP(> )
		CARD_REL_OP(< )
		CARD_REL_OP(>= )
		CARD_REL_OP(<= )
		CARD_REL_OP(!= )
		CARD_REL_OP(== )

		// Insertion operator to display Cards with cout (or any other ostream object)

		/* TODO Lab1:
				Prototype the insertion operator for the Card class.
				*/

	friend ostream& operator<<(ostream& _lhs, const Card& _rhs);


	// Needed for unit tests
	// DO NOT REMOVE
	friend class CTestManager;

};

