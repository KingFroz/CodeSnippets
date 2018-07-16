#include "Human.h"
#include "Game.h"

// Default ctor
Human::Human(const char* _name) : Player(_name)
{
	/* TODO Lab2:
	Implement this method.

	Send back the appropriate information to the parent/base class.
	*/
}

// Display the computer's name and card backs
void Human::Show() const
{
	/* TODO Lab2:
	Implement this method.

	Should display the name of the player as well as their card faces

	Example:		Human
					7♥ 3♣ 5♦ 9♥

	*/

	Card ref; 
	cout << GetName() << ": " << GetScore() << " \n";

	for (int i = 0; GetCard(i, ref); i++)
	{
		cout << ref << "  ";
	}
	
}

bool Human::Update(Stack<Card> *Pile, Deck *_obj)
{
	Card cDeck;
	const Card *MDeck = Pile->Peek();
	int choice;

	while (GetNumCards() > 0)
	{
		Console::SetCursorPosition(0, 14);
		cout << "Your Hand: ";
		Show();
		cout << endl;

		for (;;)
		{
			Console::SetCursorPosition(0, 16);
			cout << "Play a Card(Use Index): ";
			cin >> choice;
			if (GetCard(choice, cDeck))
			{
				cin.sync();
				break;
			}
			cin.clear();
			cin.sync();
		}

		

		if (cDeck.GetSuit() == MDeck->GetSuit() || cDeck.GetFace() == 12 || cDeck.GetFace() == 13)
		{
			SpecialCards(cDeck, *_obj);
			Pile->Push(cDeck);
			Discard(choice, cDeck);

			return true;
		}
		else if (cDeck.GetFace() == MDeck->GetFace())
		{
			SpecialCards(cDeck, *_obj);
			Pile->Push(cDeck);
			Discard(choice, cDeck);
			return true;
		}

		//if (Reversed(cDeck) == true)
		//	Game::ToggleReverse();
		
		_obj->Draw(cDeck);
		cout << "Drew: " << cDeck << endl;
		Sleep(1500);
		cout << endl;
	
		if (cDeck.GetSuit() == MDeck->GetSuit() || cDeck.GetFace() == MDeck->GetFace() || cDeck.GetFace() == 12 || cDeck.GetFace() == 13)
		{
			SpecialCards(cDeck, *_obj);
			Pile->Push(cDeck);
			cout << "Played: " << cDeck;
			Sleep(1500);
			return true;
		}
		else
		{
			if (Game::m_cheats & (1 << 1))
			{
				cout << "If you can't play...you Draw! Ahahaha";
				Sleep(1500);
				AddCard(cDeck);
				return false;
			}
			else
			{
				AddCard(cDeck);
				return false;
			}
		}
	}
	return 1;
}

void Human::Reversed() const
{
	Game::ToggleReverse();
}

void Human::Skip() const
{
	Game::m_skip = true;
}

void Human::Wild(Card& _temp) const
{
	char choice;
	
	for (;;)
	{
		cout << "Please choice a Color: ";
		cin >> choice;

		if (choice == 'R')
		{
			_temp.SetSuit('R');
			break;
		}
		else if (choice == 'Y')
		{
			_temp.SetSuit('Y');
			break;
		}
		else if (choice == 'B')
		{
			_temp.SetSuit('B');
			break;
		}
		else if (choice == 'G')
		{
			_temp.SetSuit('G');
			break;
		}

		cin.clear();
		cin.sync();
	}
}

void Human::Draw2() const
{
	Game::m_D2 = true;
}

void Human::Draw4() const
{
	Game::m_D4 = true;
}

bool Human::SpecialCards(Card& _temp, Deck& _pile) const
{
	switch (_temp.GetFace())
	{
	case 10:
		//cout << 'S';
		Skip();
		break;
	case 11:
		//cout << 'R';
		Reversed();
		break;
	case 12:
		//cout << 'W';
		Wild(_temp);

		break;
	case 13:
		//cout << "W4";
		Wild(_temp);
		Draw4();
		break;
	case 14:
		//cout << "D2";
		Draw2();
		break;
	default:
		return false;
		break;
	}

	return true;
}