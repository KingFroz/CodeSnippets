// Server.cpp : Contains all functions of the server.
#include "Server.h"

// Initializes the server. (NOTE: Does not wait for player connections!)
int Server::init(uint16_t port)
{
	initState();
	// TODO:
	//       1) Set up a socket for listening.
	//       2) Mark the server as active.
	svSocket = INVALID_SOCKET;
	svSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (svSocket == INVALID_SOCKET)
		return SETUP_ERROR;

	sockaddr_in sA;
	sA.sin_family = AF_INET;
	sA.sin_port = htons(port);
	sA.sin_addr.S_un.S_addr = INADDR_ANY;

	if (sA.sin_addr.S_un.S_addr == INADDR_NONE)
		return ADDRESS_ERROR;

	if (bind(svSocket, (sockaddr*)&sA, sizeof(sA)) == SOCKET_ERROR)
		return BIND_ERROR;

	if (ioctlsocket(svSocket, FIONBIO, &lock))
		return SETUP_ERROR;

	bool active = true;

	clientSequence = 0;

	return SUCCESS;
}

// Updates the server; called each game "tick".
int Server::update()
{
	// TODO: 
	//        1) Get player input and process it.
	//        2) If any player's timer exceeds 50, "disconnect" the player.
	//        3) Update the state and send the current snapshot to each player.
	NetworkMessage msg(_INPUT);
	sockaddr_in getAddr;
	int Hresult = recvfromNetMessage(svSocket, msg, &getAddr);

	if (Hresult <= 0)
	{
		if (WSAGetLastError() != EWOULDBLOCK)
			return SHUTDOWN;
	}
	else
	{
		parseMessage(getAddr, msg);
	}

	for (int i = 0; i < noOfPlayers; i++)
	{
		if (playerTimer[i] > 50)
		{
			disconnectClient(i);
		}
	}

	updateState();
	sendState();

	return SUCCESS;
}

// Stops the server.
void Server::stop()
{
	// TODO:
	//       1) Sends a "close" message to each client.
	//       2) Shuts down the server gracefully (update method should exit with SHUTDOWN code.)
	sendClose();
	shutdown(svSocket, SD_BOTH);
	closesocket(svSocket);
}

// Parses a message and responds if necessary. (private, suggested)
int Server::parseMessage(sockaddr_in& source, NetworkMessage& message)
{
	// TODO: Parse a message from client "source."
	NetworkMessage output(_OUTPUT);

	uint8_t keyUp;
	uint8_t keyDown;
	uint8_t user;

	int player = 0;
	if (source.sin_port == playerAddress[0].sin_port && playerAddress[0].sin_addr.S_un.S_addr == source.sin_addr.S_un.S_addr)
	{
		player = 0;
	}
	else if (source.sin_port == playerAddress[1].sin_port && playerAddress[0].sin_addr.S_un.S_addr == source.sin_addr.S_un.S_addr)
	{
		player = 1;
	}

	char type = message.readByte();
	switch (type)
	{
		case CL_CONNECT:
			if (noOfPlayers < 2)
			{
				sendOkay(source);
				user = message.readByte();
				connectClient(user, source);
			}
			else
			{
				sendFull(source);
			}

			break;
		case CL_KEYS:
			keyUp = message.readByte();
			keyDown = message.readByte();

			if (player == 0)
			{
				state.player0.keyUp = keyUp;
				state.player0.keyDown = keyDown;
			}
			else if (player == 1)
			{
				state.player1.keyUp = keyUp;
				state.player1.keyDown = keyDown;
			}

			break;
		case CL_ALIVE:
			if (player == 0)
				playerTimer[player] = 0;
			else if (player == 1)
			{
				playerTimer[player] = 0;
			}
			break;
		case SV_CL_CLOSE:
			disconnectClient(player);
			break;
	}
	
	return SUCCESS;
}

// Sends the "SV_OKAY" message to destination. (private, suggested)
int Server::sendOkay(sockaddr_in& destination)
{
	// TODO: Send "SV_OKAY" to the destination.
	NetworkMessage output(_OUTPUT);
	output.writeShort(clientSequence);
	output.writeByte(SV_OKAY);
	int iResult = sendMessage(destination, output);

	return SUCCESS;
}

// Sends the "SV_FULL" message to destination. (private, suggested)
int Server::sendFull(sockaddr_in& destination)
{
	// TODO: Send "SV_FULL" to the destination.
	NetworkMessage output(_OUTPUT);
	output.writeShort(clientSequence);
	output.writeByte(SV_FULL);
	int iResult = sendMessage(destination, output);

	return SUCCESS;
}

// Sends the current snapshot to all players. (private, suggested)
int Server::sendState()
{
	// TODO: Send the game state to each client.
	for (int i = 0; i < noOfPlayers; i++)
	{
		uint8_t type = SV_SNAPSHOT;
		uint8_t phase = state.gamePhase;

		int16_t ballX = state.ballX;
		int16_t ballY = state.ballY;

		int16_t p0_y = state.player0.y;
		int16_t p0_Score = state.player0.score;

		int16_t p1_y = state.player1.y;
		int16_t p1_Score = state.player1.score;

		NetworkMessage output(_OUTPUT);

		output.writeShort(clientSequence);

		output.writeByte(type);
		output.writeByte(phase);

		output.writeShort(ballX);
		output.writeShort(ballY);

		output.writeShort(p0_y);
		output.writeShort(p0_Score);

		output.writeShort(p1_y);
		output.writeShort(p1_Score);

		int iResult = sendMessage(playerAddress[i], output);
	}

	return SUCCESS;
}

// Sends the "SV_CL_CLOSE" message to all clients. (private, suggested)
void Server::sendClose()
{
	// TODO: Send the "SV_CL_CLOSE" message to each client
	for (int i = 0; i < noOfPlayers; i++)
	{
		NetworkMessage output(_OUTPUT);
		output.writeShort(clientSequence);
		output.writeByte(SV_CL_CLOSE);

		int iResult = sendMessage(playerAddress[i], output);
	}
}

// Server message-sending helper method. (private, suggested)
int Server::sendMessage(sockaddr_in& destination, NetworkMessage& message)
{
	// TODO: Send the message in the buffer to the destination.
	if (sendtoNetMessage(svSocket, message, &destination) <= 0) { return MESSAGE_ERROR; }
	clientSequence++;
	return SUCCESS;
}

// Marks a client as connected and adjusts the game state.
void Server::connectClient(int player, sockaddr_in& source)
{
	playerAddress[player] = source;
	playerTimer[player] = 0;

	noOfPlayers++;
	if (noOfPlayers == 1)
		state.gamePhase = WAITING;
	else
		state.gamePhase = RUNNING;
}

// Marks a client as disconnected and adjusts the game state.
void Server::disconnectClient(int player)
{
	playerAddress[player].sin_addr.s_addr = INADDR_NONE;
	playerAddress[player].sin_port = 0;

	noOfPlayers--;
	if (noOfPlayers == 1)
		state.gamePhase = WAITING;
	else
		state.gamePhase = DISCONNECTED;
}

// Updates the state of the game.
void Server::updateState()
{
	// Tick counter.
	static int timer = 0;

	// Update the tick counter.
	timer++;

	// Next, update the game state.
	if (state.gamePhase == RUNNING)
	{
		// Update the player tick counters (for ALIVE messages.)
		playerTimer[0]++;
		playerTimer[1]++;

		// Update the positions of the player paddles
		if (state.player0.keyUp)
			state.player0.y--;
		if (state.player0.keyDown)
			state.player0.y++;

		if (state.player1.keyUp)
			state.player1.y--;
		if (state.player1.keyDown)
			state.player1.y++;
		
		// Make sure the paddle new positions are within the bounds.
		if (state.player0.y < 0)
			state.player0.y = 0;
		else if (state.player0.y > FIELDY - PADDLEY)
			state.player0.y = FIELDY - PADDLEY;

		if (state.player1.y < 0)
			state.player1.y = 0;
		else if (state.player1.y > FIELDY - PADDLEY)
			state.player1.y = FIELDY - PADDLEY;

		//just in case it get stuck...
		if (ballVecX)
			storedBallVecX = ballVecX;
		else
			ballVecX = storedBallVecX;

		if (ballVecY)
			storedBallVecY = ballVecY;
		else
			ballVecY = storedBallVecY;

		state.ballX += ballVecX;
		state.ballY += ballVecY;

		// Check for paddle collisions & scoring
		if (state.ballX < PADDLEX)
		{
			// If the ball has struck the paddle...
			if (state.ballY + BALLY > state.player0.y && state.ballY < state.player0.y + PADDLEY)
			{
				state.ballX = PADDLEX;
				ballVecX *= -1;
			}
			// Otherwise, the second player has scored.
			else
			{
				newBall();
				state.player1.score++;
				ballVecX *= -1;
			}
		}
		else if (state.ballX >= FIELDX - PADDLEX - BALLX)
		{
			// If the ball has struck the paddle...
			if (state.ballY + BALLY > state.player1.y && state.ballY < state.player1.y + PADDLEY)
			{
				state.ballX = FIELDX - PADDLEX - BALLX - 1;
				ballVecX *= -1;
			}
			// Otherwise, the first player has scored.
			else
			{
				newBall();
				state.player0.score++;
				ballVecX *= -1;
			}
		}

		// Check for Y position "bounce"
		if (state.ballY < 0)
		{
			state.ballY = 0;
			ballVecY *= -1;
		}
		else if (state.ballY >= FIELDY - BALLY)
		{
			state.ballY = FIELDY - BALLY - 1;
			ballVecY *= -1;
		}
	}

	// If the game is over...
	if ((state.player0.score > 10 || state.player1.score > 10) && state.gamePhase == RUNNING)
	{
		state.gamePhase = GAMEOVER;
		timer = 0;
	}
	if (state.gamePhase == GAMEOVER)
	{
		if (timer > 30)
		{
			initState();
			state.gamePhase = RUNNING;
		}
	}
}

// Initializes the state of the game.
void Server::initState()
{
	playerAddress[0].sin_addr.s_addr = INADDR_NONE;
	playerAddress[1].sin_addr.s_addr = INADDR_NONE;
	playerTimer[0] = playerTimer[1] = 0;
	noOfPlayers = 0;

	state.gamePhase = DISCONNECTED;

	state.player0.y = 0;
	state.player1.y = FIELDY - PADDLEY - 1;
	state.player0.score = state.player1.score = 0;
	state.player0.keyUp = state.player0.keyDown = false;
	state.player1.keyUp = state.player1.keyDown = false;

	newBall(); // Get new random ball position

	// Get a new random ball vector that is reasonable
	ballVecX = (rand() % 10) - 5;
	if ((ballVecX >= 0) && (ballVecX < 2))
		ballVecX = 2;
	if ((ballVecX < 0) && (ballVecX > -2))
		ballVecX = -2;

	ballVecY = (rand() % 10) - 5;
	if ((ballVecY >= 0) && (ballVecY < 2))
		ballVecY = 2;
	if ((ballVecY < 0) && (ballVecY > -2))
		ballVecY = -2;


}

// Places the ball randomly within the middle half of the field.
void Server::newBall()
{
	// (randomly in 1/2 playable area) + (1/4 playable area) + (left buffer) + (half ball)
	state.ballX = (rand() % ((FIELDX - 2 * PADDLEX - BALLX) / 2)) +
						((FIELDX - 2 * PADDLEX - BALLX) / 4) + PADDLEX + (BALLX / 2);

	// (randomly in 1/2 playable area) + (1/4 playable area) + (half ball)
	state.ballY = (rand() % ((FIELDY - BALLY) / 2)) + ((FIELDY - BALLY) / 4) + (BALLY / 2);
}
