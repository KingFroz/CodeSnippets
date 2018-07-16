#pragma once
#include "stdafx.h"
#include "Player.h"

/* TODO Lab2:
		Make Human a child of Player
*/

class Human : public Player
{

public:
	// Default ctor
	// In:	_name		The name of the human player

	/* TODO Lab2:
	Set the default value of _name to "Human"
	*/
	Human(const char* _name = "Human");

	// Display the computer's name and card backs
	void Show() const;

	void Reversed() const;
	void Draw4() const;
	void Skip() const;
	void Draw2() const;
	void Wild(Card& _temp) const;

	bool SpecialCards(Card& _temp, Deck& _pile) const;

	bool Update(Stack<Card> *Pile, Deck* _obj);
	// Needed for unit tests
	// DO NOT REMOVE
	friend class CTestManager;
};
