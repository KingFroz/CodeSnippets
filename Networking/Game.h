#pragma once
#include <iostream>
using namespace std;

#include "Player.h"
#include "Deck.h"
#include "Human.h"
#include "Computer.h"
#include "Player.h"
#include "Conio.h"
#include <fstream>
#include <vector>
// Enum of gamestates, you can add and remove from this if you need additional ones (examples include: GAME_TITLE, GAME_MENU, GAME_OPTIONS, etc).
enum GAMESTATE { GAME_INIT, GAME_TITLE, GAME_MENU, GAME_PLAY, GAME_END, ROUND_INIT, ROUND_END, GAME_SCORES};

class Game
{
	struct ScoreInfo
	{
		char name[32];
		int score;
	};

	// Necessary data members
	Stack<Card> Pile;
	Player* m_players[4];	// The players in the game
	Deck m_Deck;			// The deck of cards
	//int m_currPlayer = 0;		// Keeps track of which index is currently taking their turn
	int m_numPlayers;		// The number of players playing in the game
	int m_arrow = 1;
	vector <ScoreInfo> scores;

	// If you're coding UNO, this data member is necessary as well
public:
	static bool m_reversed;			// Keeps track of the order of play
	static bool m_skip;
	static bool m_D2;
	static bool m_D4;
	static unsigned char m_cheats;
	bool currGame = false;
private:
	int GetNumPlayers() { return m_numPlayers; }
	// GAMESTATE object to store the game's current state.
	GAMESTATE m_state;

	// Add game data members as necessary

public:
	// Default ctor
	Game();

	// Dtor
	~Game();

	// The main function of the game, this loops until a certain gamestate is set.
	void Run();

	// Modifier for the gamestate data member.
	void SetState(GAMESTATE _state) { m_state = _state; }

	static void ToggleReverse() { m_reversed = !m_reversed; }
	// Get the score for the game
	//	For Go Fish, this will take care of pairing
	//  For UNO, this will be used for end of round scoring.
	// int Score(Player* _player);
	void Title();
	
	bool Uno();
	
	void ToggleCodes(Stack <int> *input);

	void InputScroll();

	void DeckTop();

	void Reshuffle();

	void DropHand();

	void ReplayToggle();

	void ScoreSave(const char * _name, int _score);

	void LoadScores();

	
};

