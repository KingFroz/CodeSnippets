#include "Game.h"

// Default ctor
bool Game::m_reversed = false;
bool Game::m_skip = false;
bool Game::m_D2 = false;
bool Game::m_D4 = false;
unsigned char Game::m_cheats = 0;

void Game::ScoreSave(const char * _name, int _score)
{
	fstream file;

	file.open("HighScore.txt", ios::out | ios::app);

	if (file.is_open())
	{
		file << _name << "\n";
		file << _score << "\n";
		file.close();
	}

}

void Game::LoadScores()
{
	fstream loadfile;

	loadfile.open("HighScore.txt", ios::in);

	if (loadfile.is_open())
	{
		while (!loadfile.eof())
		{
			ScoreInfo info;

			loadfile >> info.name;
			loadfile >> info.score;
			scores.push_back(info);
		}
		scores.pop_back();
		loadfile.close();
	}

}

void Game::Title()
{
	Console::SetCursorPosition(5, 1);		
	Console::ForegroundColor(Cyan);
	Console::BackgroundColor(Blue);
	cout << "          _____                    _____                   _______         ";
	Console::SetCursorPosition(5, 2);													 
	cout << "         /\\    \\                  /\\    \\                 /::\\    \\        ";
	Console::SetCursorPosition(5, 3);													 
	cout << "        /::\\____\\                /::\\____\\               /::::\\    \\       ";
	Console::SetCursorPosition(5, 4);													 
	cout << "       /:::/    /               /::::|   |              /::::::\\    \\      ";
	Console::SetCursorPosition(5, 5);													 
	cout << "      /:::/    /               /:::::|   |             /::::::::\\    \\     ";
	Console::SetCursorPosition(5, 6);													 
	cout << "     /:::/    /               /::::::|   |            /:::/~~\\:::\\    \\    ";
	Console::SetCursorPosition(5, 7);													 
	cout << "    /:::/    /               /:::/|::|   |           /:::/    \\:::\\    \\   ";
	Console::SetCursorPosition(5, 8);													 
	cout << "   /:::/    /               /:::/ |::|   |          /:::/    / \\:::\\    \\  ";
	Console::SetCursorPosition(5, 9);													 
	cout << "  /:::/    /      _____    /:::/  |::|   | _____   /:::/____/   \\:::\\____\\ ";
	Console::SetCursorPosition(5, 10);													 
	cout << " /:::/____/      /\\    \\  /:::/   |::|   |/\\    \\ |:::|    |     |:::|    |";
	Console::SetCursorPosition(5, 11);													 
	cout << "|:::|    /      /::\\____\\/:: /    |::|   /::\\____\\|:::|____|     |:::|    |";
	Console::SetCursorPosition(5, 12);													 
	cout << "|:::|____\\     /:::/    /\\::/    /|::|  /:::/    / \\:::\\    \\   /:::/    / ";
	Console::SetCursorPosition(5, 13);													 
	cout << " \\:::\\    \\   /:::/    /  \\/____/ |::| /:::/    /   \\:::\\    \\ /:::/    /  ";
	Console::SetCursorPosition(5, 14);													 
	cout << "  \\:::\\    \\ /:::/    /           |::|/:::/    /     \\:::\\    /:::/    /   ";
	Console::SetCursorPosition(5, 15);													 
	cout << "   \\:::\\    /:::/    /            |::::::/    /       \\:::\\__/:::/    /    ";
	Console::SetCursorPosition(5, 16);													 
	cout << "    \\:::\\__/:::/    /             |:::::/    /         \\::::::::/    /     ";
	Console::SetCursorPosition(5, 17);													 
	cout << "     \\::::::::/    /              |::::/    /           \\::::::/    /      ";
	Console::SetCursorPosition(5, 18);													 
	cout << "      \\::::::/    /               /:::/    /             \\::::/    /       ";
	Console::SetCursorPosition(5, 19);													 
	cout << "       \\::::/    /               /:::/    /               \\::/____/        ";
	Console::SetCursorPosition(5, 20);													 
	cout << "        \\::/____/                \\::/    /                 ~~              ";
	Console::SetCursorPosition(5, 21);													 
	cout << "         ~~                       \\/____/                                  ";
	
	Console::ResetColor();
	Console::SetCursorPosition(30, 25);
	cout << "Created by: LISTON HARDING";
	Console::Lock(true);
}


void Game::ReplayToggle()
{
	Console::Clear();
	cout << "CHEATS" << "\n"
		<< "--------------------------------------------------- \n"
		<< "Show cards: Shift then 1" << "\n"
		<< "Blinded/taunt: Shift then 2" << "\n"
		<< "Doom: Shift then 3" << "\n"
		<< "Continue: Hit SPACE" << endl << endl;

	cout << "Enter a Code: ";
	Stack<int> ResetCodes;


	for (;;)
	{
		if (GetAsyncKeyState(VK_SHIFT) && ResetCodes.GetCurrSize() == 0)
		{
			ResetCodes.Push(10);
		}

		if (GetAsyncKeyState(VK_NUMPAD1) && ResetCodes.GetCurrSize() > 0 && * ResetCodes.Peek() == 10)
		{
			ResetCodes.Push(61);
		}

		if (GetAsyncKeyState(VK_NUMPAD2) && ResetCodes.GetCurrSize() > 0 && * ResetCodes.Peek() == 10)
		{
			ResetCodes.Push(62);
		}

		if (GetAsyncKeyState(VK_NUMPAD3) && ResetCodes.GetCurrSize() > 0 && * ResetCodes.Peek() == 10)
		{
			ResetCodes.Push(63);
		}


		if (GetAsyncKeyState(VK_SPACE))
		{
			m_state = GAME_PLAY;
			break;
		}

		ToggleCodes(&ResetCodes);
		Sleep(100);
		Console::FlushKeys();
	}
}

void Game::DropHand()
{
	for (int i = 0; i < GetNumPlayers(); i++)
	{
		for (int x = 0; x < m_players[i]->GetNumCards(); x++)
		{
		Card TemHand;

		m_players[i]->GetCard(x, TemHand);
		m_Deck.AddCardToDeck(TemHand);
		}

		m_players[i]->Clear();
	}
}

void Game::ToggleCodes(Stack <int> *input)
{
	int fKey, sKey;

	if (input->GetCurrSize() < 2)
	{
		return;
	}
	else
	{
		input->Pop(sKey);
		input->Pop(fKey);

	}

	if (fKey == 10)
	{
		switch (sKey)
		{
		case 61:
			Game::m_cheats = Game::m_cheats ^ (1 << 0);
			cout << "Cheat toggled - Peekaboo!";
			cout << endl;
			break;

		case 62:
			Game::m_cheats = Game::m_cheats ^ (1 << 1);
			cout << "Cheat toggled - Lets Play a Game....";
			cout << endl;
			break;
		case 63:
			Game::m_cheats = Game::m_cheats ^ (1 << 2);
			cout << "Cheat toggled - Impossible Mode!";
			cout << endl;
			break;
		default:
			break;
		}

	}
	input->Clear();
}

void Game::Reshuffle()
{
	while (Pile.GetCurrSize() > 0)
	{
		Card TemPile;

		Pile.Pop(TemPile);
		m_Deck.AddCardToDeck(TemPile);
	}

}

void Game::InputScroll()
{
	if (GetAsyncKeyState(VK_UP))
	{
		m_arrow--;

		if (m_arrow < 1)
		{
			m_arrow = 5;
		}
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		m_arrow++;

		if (m_arrow > 5)
		{
			m_arrow = 1;
		}
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		switch (m_arrow)
		{
		case 1:
			if (currGame == false)
			{
				Console::SetCursorPosition(20, 11);
				cout << "  Please enter the amount of Players\n";
				Console::SetCursorPosition(20, 12);
				cout << "(INCLUDING AMOUNT OF COMPUTER PLAYERS)";
				Sleep(1500);
				Console::Clear();


				for (;;)
				{
					cout << "How many players (2-4)?: ";
					if (cin >> m_numPlayers && m_numPlayers > 1 && m_numPlayers < 5)
					{
						cin.sync();
						break;
					}
					cin.clear();
					cin.sync();
				}

				Console::Clear();

				int choice;
				for (int i = 0; i < m_numPlayers; i++)
				{
					cout << "Player #" << i + 1 << '\n';
					cout << "1) Human\n"
						<< "2) Computer\n";
					for (;;)
					{
						cout << "Pick a class: ";
						if (cin >> choice && choice >= 1 && choice <= 2)
						{
							cin.sync();
							break;
						}
						cin.clear();
						cin.sync();
					}


					switch (choice)
					{
					case 1:
						cout << endl << endl;
						m_players[i] = new Human;
						char buffer[64];

						for (;;)
						{
							cout << "Enter your Name: ";
							if (cin.get(buffer, 64))
							{
								cin.sync();
								break;
							}
							cin.clear();
							cin.sync();
						}

						m_players[i]->SetName(buffer);
						break;
					case 2:
						m_players[i] = new Computer;
						break;
					}

					m_players[i]->GetName();
					Console::Clear();
				}
			}
			
			m_state = GAME_PLAY;
			break;
		case 2:
			Console::Clear();
			Console::SetCursorPosition(33, 2);
			cout << "RULES \n\n"
				<< "---------------------------------------------------------------------------- \n"
				<< "Each Player is dealt 7 cards, the remaining one starts the Draw Pile." << endl
				<< "Players have to match the card in the Discard Pile either by number, " << endl
				<< "color or word." << endl << endl
				<< "If a player can't match that card, they must take a card from the Draw Pile." << endl
				<< "If the Card drawn matches the Draw Pile, it must be played." << endl
				<< "Otherwise play moves to the next player." << endl
				<< endl
				<< "When a player has one Card left, they can close the game by declaring UNO," << endl
				<< "Before Disarding there last Card!" << endl << endl
				<< "\t\t\t\t Enjoy!!! " << "\n\n";

			system("pause");
			m_arrow = 1;
			Console::Clear();
			break;
		case 3:
		{
			Console::Clear();
			cout << "CHEATS" << "\n"
				<< "--------------------------------------------------- \n"
				<< "Show cards : Shift then 1" << "\n"
				<< "Blinded/Taunt: Shift then 2" << "\n"
				<< "Doom: Shift then 3" << "\n"
				<< "Exit: Hit Esc" << endl << endl;

			cout << "Enter a Code: ";
			Stack<int> Codes;


			for (;;)
			{

				if (GetAsyncKeyState(VK_SHIFT) && Codes.GetCurrSize() == 0)
				{
					Codes.Push(10);
				}

				if (GetAsyncKeyState(VK_NUMPAD1) && Codes.GetCurrSize() > 0 && * Codes.Peek() == 10)
				{
					Codes.Push(61);
				}

				if (GetAsyncKeyState(VK_NUMPAD2) && Codes.GetCurrSize() > 0 && * Codes.Peek() == 10)
				{
					Codes.Push(62);
				}

				if (GetAsyncKeyState(VK_NUMPAD3) && Codes.GetCurrSize() > 0 && * Codes.Peek() == 10)
				{
					Codes.Push(63);
				}


				if (GetAsyncKeyState(VK_ESCAPE))
				{
					m_state = GAME_MENU;
					break;
				}

				ToggleCodes(&Codes);
				Sleep(100);
				Console::FlushKeys();

			}
			Console::Clear();
		}

			break;
		case 4:
			m_state = GAME_END;
			break;
		case 5:
			m_state = GAME_SCORES;
		default:
			break;
		}
		Console::FlushKeys();
	}
}

Game::Game()
{
	m_state = GAME_INIT;
}

// Dtor
Game::~Game()
{
	for (int i = 0; i < m_numPlayers; ++i)
		delete m_players[i];
}

bool Game::Uno()
{
	const char* uno;

	for (int i = 0; i < GetNumPlayers() - 1; i++)
	{
		m_players[i]->TallyScore();
	}

	int lowerScore = 500;
	bool winner = false;
	
	for (int x = 0; x < GetNumPlayers(); x++)
	{
		if (m_players[x]->GetScore() < lowerScore)
		{
			lowerScore = m_players[x]->GetScore();
			uno = m_players[x]->GetName();
		}

		if (m_players[x]->GetScore() >= 100)
		{
			winner = true;
		}
	}

	if (winner == true)
	{
		Console::Clear();
		cout << "You Win: " << uno << endl << endl;
		ScoreSave(uno, lowerScore);
		system("pause");
		return true;
	}
	else
	{
		return false;
	}
	

}

void Game::DeckTop()
{
	if (Game::m_cheats & (1 << 2))
	{
		Console::SetCursorPosition(62, 0);
		cout << "Deck Top: XX";
	}
	else
	{
		Console::SetCursorPosition(60, 0);
		cout << "Deck Top: ";
		cout << *(Pile.Peek());
	}
}

void Game::Run()
{
	// Bool to control whether the game should continue to run.
	bool bRun = true;
	bool menu = true;
	// Loop while our bool remains true.
	while(bRun)
	{
		switch(m_state)
		{
		case GAME_INIT:
			// Insert initialization code here.
			m_Deck.Initialize();
			m_Deck.Shuffle();

			Title();
			Sleep(2000);
			m_state = GAME_MENU;
			break;
		case GAME_MENU:
			// Insert menu code here.
			Console::Clear();

			while (menu)
			{

				Console::Lock(true);
				cout << "Main Menu: [Space to Enter]" << "\n\n";
				m_arrow == 1 ? Console::BackgroundColor(Blue) : Console::BackgroundColor(Black);
				cout << "1) Play" << "\n";
				m_arrow == 2 ? Console::BackgroundColor(Blue) : Console::BackgroundColor(Black);
				cout << "2) Intructions" << "\n";
				m_arrow == 3 ? Console::BackgroundColor(Blue) : Console::BackgroundColor(Black);
				cout << "3) Cheats" << "\n";
				m_arrow == 4 ? Console::BackgroundColor(Blue) : Console::BackgroundColor(Black);
				cout << "4) Exit" << "\n";
				m_arrow == 5 ? Console::BackgroundColor(Blue) : Console::BackgroundColor(Black);
				cout << "5) HIGHSCORES" << endl << endl;

				Console::BackgroundColor(Black);
				Console::SetCursorPosition(0, 0);
				Console::Lock(false);
				InputScroll();
				Sleep(100);

				if (m_state != GAME_MENU)
				{
					break;
				}
			}
			break;
		case GAME_PLAY:
		{
			Console::Clear();
			// Insert game play code here.
			if (currGame == false)
			{
				for (int i = 0; i < 7; i++)
				{
					Console::Clear();
					for (int s = 0; s < m_numPlayers; s++)
					{
						Card deal;

						m_Deck.Draw(deal);
						m_players[s]->AddCard(deal);
						if (s == 0)
						{
							cout << "*";
						}
						m_players[s]->Sort();
						m_players[s]->Show();

						cout << endl << endl;



					}
				}
			}
			else
			{
				Console::Clear();
				for (int i = 0; i < m_numPlayers; i++)
				{
					m_players[i]->Show();
					cout << endl << endl;
				}
			}
			
			if (currGame == false)
			{
				Card top;
				m_Deck.Draw(top);

				Pile.Push(top);
			}
			
			bool GameOver = true;
			bool MenuJump = false;
			while (GameOver && MenuJump == false)
			{
				
				for ( int i = 0; i < m_numPlayers; )
				{

					if (GetAsyncKeyState(VK_ESCAPE))
					{
						m_state = GAME_MENU;
						MenuJump = true;
						currGame = true;
						break;
					}

					Console::FlushKeys();

					if (m_Deck.IsEmpty())
						Reshuffle();	

					DeckTop();
					
					if (m_players[i]->GetNumCards() != 0)
					{
						m_players[i]->Sort();
						m_players[i]->Update(&Pile, &m_Deck);
						cout << "\n\n";

						Console::Clear();

						//m_players[1]->Clear();

						if (m_reversed == true)
						{
							i--;
							if (m_skip == true)
							{
								i--;
							}
						}
						else
						{
							i++;
							if (m_skip == true)
							{
								i++;
							}
							
						}

						m_skip = false;

						if (i == m_numPlayers)
								i = 0;

						else if (i < 0)
						{
							i = m_numPlayers - 1;
						}

						if (m_D2)
						{
							Card tmp;

							for (int j = 0; j < 2; j++)
							{
								
								m_Deck.Draw(tmp);
								m_players[i]->AddCard(tmp);
							}

							Console::SetCursorPosition(35, 10);
							cout << m_players[i]->GetName();
							Console::SetCursorPosition(26, 11);
							cout <<  " ----- Drew 2 cards -----";
							Sleep(2000);
							Console::Clear();
							m_D2 = false;
						}
						else if (m_D4)
						{
							Card tmp;


							for (int j = 0; j < 4; j++)
							{

								m_Deck.Draw(tmp);
								m_players[i]->AddCard(tmp);
							}
							Console::SetCursorPosition(35, 10);
							cout << m_players[i]->GetName();
							Console::SetCursorPosition(26, 11);
							cout << " ----- Drew 4 cards -----";
							Sleep(2000);
							Console::Clear();
							m_D4 = false;
						}
						
						for (int x = 0; x < m_numPlayers; x++)
						{
							if (x == i)
							{
								cout << "*";
							}
							m_players[x]->Sort();
							m_players[x]->Show();
							cout << "\n\n";
						}
						
					}
					else
					{
						if (Uno() == false)
						{
							Card Uno;
							DropHand();
							Reshuffle();

							for (int i = 0; i < 7; i++)
							{
								Console::Clear();
								for (int s = 0; s < m_numPlayers; s++)
								{
									Card deal;

									m_Deck.Draw(deal);
									m_players[s]->AddCard(deal);
									if (s == 0)
									{
										cout << "*";
									}
									m_players[s]->Show();

									cout << endl << endl;
								}
							}

							m_Deck.Draw(Uno);
							Pile.Push(Uno);
						}
						else
						{
							GameOver = false;
							break;
						}
					}
				}

			}

			if (MenuJump == false)
			{
				char replay;
				Console::Clear();
				Console::SetCursorPosition(30, 11);

				for (;;)
				{
					cout << "Replay? (y/n): ";
					if (cin >> replay && tolower(replay) == 'y'
						|| tolower(replay) == 'n')
					{
						cin.sync();
						break;
					}
					cin.clear();
					cin.sync();
					Console::Clear();
					Console::SetCursorPosition(30, 11);
				}

				if ('y' == tolower(replay))
				{
					DropHand();
					Reshuffle();
					ReplayToggle();
					
					for (int i = 0; i < GetNumPlayers(); i++)
					{
						m_players[i]->ResetScore();
					}

					break;
				}
				else
				{
					for (int i = 0; i < m_numPlayers; i++)
					{
						delete m_players[i];
					}
					m_state = GAME_MENU;
					break;
				}
			}
		}
		break;
		case GAME_END:
			// The game is over, change the bool to stop the loop.
			Console::SetCursorPosition(5, 10);
			bRun = false;
			break;
		case GAME_SCORES:
			LoadScores();
			Console::Clear();

			for (int i = 0; i < scores.size(); i++)
			{
				cout << scores[i].name << ": " << scores[i].score << "\n";
			}
			system("pause");
			scores.clear();
			m_state = GAME_MENU;
			Console::FlushKeys();
			break;
		}
		
	}
}