#include "Computer.h"

// Default ctor
Computer::Computer(const char* _name) : Player(_name)
{
	/* TODO Lab2:
			Implement this method.
	
			Send back the appropriate information to the parent/base class.
	*/
}

// Display the computer's name and card backs
void Computer::Show() const
{
	/* TODO Lab2:
			Implement this method.
	
			Should display the name of the player as well as their card backs.

			Example:		Computer
							XX XX XX XX

	*/
	cout << GetName() << ": " << GetScore()<< "\n";

	if (Game::m_cheats & (1 << 0))
	{
		Card temp;

		for (int x = 0; GetCard(x, temp); x++)
		{
			cout << temp << "  ";
		}
	}
	else
	{
		for (int start = 0; start < GetNumCards(); start++)
		{
			cout << "XX" << " ";
		}
	}
}

bool Computer::Update(Stack<Card> *Pile, Deck *_obj)
{
	Card cDeck;

	const Card *MDeck = Pile->Peek();

	for (int i = 0; i < GetNumCards(); i++)
	{
		GetCard(i, cDeck);
		Console::SetCursorPosition(0, 14);
		if (cDeck.GetSuit() == MDeck->GetSuit() || cDeck.GetFace() == 12 || cDeck.GetFace() == 13)
		{
			cout << Computer::GetName();
			cout << " Played: " << cDeck;
			Sleep(1500);
			SpecialCards(cDeck, *_obj);
			Pile->Push(cDeck);
			Discard(i, cDeck);
			return true;
		}
		else if (cDeck.GetFace() == MDeck->GetFace())
		{
			cout << Computer::GetName();
			cout << " Played: " << cDeck;
			Sleep(1500);
			SpecialCards(cDeck, *_obj);
			Pile->Push(cDeck);
			Discard(i, cDeck);
			return true;
		}
	}

	Console::SetCursorPosition(0, 14);
	_obj->Draw(cDeck);
	cout << "Computer Drew: XX" << endl;
	Sleep(1500);
	cout << endl;


	if (cDeck.GetSuit() == MDeck->GetSuit() || cDeck.GetFace() == MDeck->GetFace() || cDeck.GetFace() == 12 || cDeck.GetFace() == 13)
	{
		Console::SetCursorPosition(0, 16);
		cout << Computer::GetName();
		cout << " Played: " << cDeck;
		Sleep(1500);
		SpecialCards(cDeck, *_obj);
		Pile->Push(cDeck);

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
	return 1;
}


void Computer::Reversed() const
{
	Game::ToggleReverse();
}

void Computer::Skip() const
{
	Game::m_skip = true;
}

void Computer::Wild(Card& _temp) const
{
	switch (rand() % 4)
	{
	case 0:
		_temp.SetSuit('B');
		Console::SetCursorPosition(0, 18);
		cout << GetName() << " Set the Color to Blue!";
		Sleep(1000);
		break;
	case 1:
		_temp.SetSuit('G');
		Console::SetCursorPosition(0, 18);
		cout << GetName() << " Set the Color to Green!";
		Sleep(1000);
		break;
	case 2:
		_temp.SetSuit('R');
		Console::SetCursorPosition(0, 18);
		cout << GetName() << " Set the Color to Red!";
		Sleep(1000);
		break;
	case 3: 
		_temp.SetSuit('Y');
		Console::SetCursorPosition(0, 18);
		cout << GetName() << " Set the Color to Yellow!";
		Sleep(1000);
		break;
	}
}

void Computer::Draw2() const
{
	Game::m_D2 = true;
}

void Computer::Draw4() const
{
	Game::m_D4 = true;
}

bool Computer::SpecialCards(Card& _temp, Deck& _pile) const
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