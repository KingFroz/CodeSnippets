#include "Card.h"

// Default constructor
// In:	_face			The face value of the card
//		_suit			The suit of the card
Card::Card(int _face, char _suit)
{
	/* TODO Lab1:
			Fill out the constructor body
	*/
	m_face = _face;
	m_suit = _suit;
}

Card::~Card()
{

}


/* Accessors */

/* TODO Lab1:
		Define the two accessors if they were not inlined.
*/

/* Mutators */

/* TODO Lab1:
		Define the two mutators if they were not inlined.
*/

// Display a card to the screen
void Card::Show() const
{
	/* TODO Lab1:
			Display the Card object.  

			Make sure that "face" cards (Jack-Ace) display as 'J', 'Q', 'K', 'A'
			or something similar instead of their numeric value.

			*Note - This unit test is not automated, so make sure you verify 
			the face card is displaying correctly.
	*/

	switch (m_suit)
	{
	case 'B':
		Console::ForegroundColor(Blue);
		cout << m_suit;
		Console::ResetColor();
		break;
	case 'G':
		Console::ForegroundColor(Green);
		cout << m_suit; 
		Console::ResetColor();
		break;
	case 'Y':
		Console::ForegroundColor(Yellow);
		cout << m_suit;
		Console::ResetColor();
		break;
	case 'R':
		Console::ForegroundColor(Red);
		cout << m_suit;
		Console::ResetColor();
		break;
	}

	switch (m_face)
	{
	case 10:
		cout << 'S';
		break;
	case 11:
		cout << 'R';
		break;
	case 12:
		cout << 'W';
		break;
	case 13:
		cout << "W4";
		break;
	case 14:
		cout << "D2";
		break;
	default:
		cout << m_face;
		break;
	}
	cout << " ";

}

// Relational operators

/* TODO Lab1:
		Define the six relational operators if they were not inlined.
*/

// Insertion operator to display Cards with cout (or any other ostream object)

/* TODO Lab1:
		Define the insertion operator to display a card.

		Make sure that "face" cards (Jack-Ace) display as 'J', 'Q', 'K', 'A'
		or something similar instead of their numeric value.

		*Note - This unit test is not automated, so make sure you verify
		the face card is displaying correctly.
*/
ostream& operator<<(ostream& _lhs, const Card& _rhs)
{
	switch (_rhs.m_suit)
	{
	case 'B':
		Console::ForegroundColor(Blue);
		_lhs << _rhs.m_suit;
		Console::ResetColor();
		break;
	case 'G':
		Console::ForegroundColor(Green);
		_lhs << _rhs.m_suit;
		Console::ResetColor();
		break;
	case 'Y':
		Console::ForegroundColor(Yellow);
		_lhs << _rhs.m_suit;
		Console::ResetColor();
		break;
	case 'R':
		Console::ForegroundColor(Red);
		_lhs << _rhs.m_suit;
		Console::ResetColor();
		break;
	}
	
	switch (_rhs.m_face)
	{
	case 10:
		_lhs << 'S';
		break;
	case 11:
		_lhs << 'R';
		break;
	case 12:
		_lhs << 'W';
		break;
	case 13:
		_lhs << "W4";
		break;
	case 14:
		_lhs << "D2";
		break;
	default:
		_lhs << _rhs.m_face;
		break;
	}

	return _lhs;
}
