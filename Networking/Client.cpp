// Client.cpp : This file handles all client network functions.
#include "Client.h"
#include "../NetworkMessage.h"

// Initializes the client; connects to the server.
int Client::init(char* address, uint16_t port, uint8_t _player)
{
	state.player0.keyUp = state.player0.keyDown = false;
	state.player1.keyUp = state.player1.keyDown = false;
	state.gamePhase = WAITING;
	// TODO:
	//       1) Set the player. CHECK
	//       2) Set up the connection. CHECK
	//       3) Connect to the server. CHECK
	//       4) Get response from server. CHECK
	//       5) Make sure to mark the client as running. CHECK
	player = _player;
	snapCount = 0;

	clSocket = INVALID_SOCKET;
	clSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clSocket == INVALID_SOCKET)
		return SETUP_ERROR;

	sA.sin_family = AF_INET;
	sA.sin_port = htons(port);
	sA.sin_addr.S_un.S_addr = inet_addr(address);

	if (sA.sin_addr.S_un.S_addr == INADDR_NONE)
		return ADDRESS_ERROR;

	if (connect(clSocket, (sockaddr*)&sA, sizeof(sA)) == SOCKET_ERROR)
	{
		return DISCONNECT;
	}

	//SENDING
	NetworkMessage output(_OUTPUT);
	output.writeByte(CL_CONNECT);
	output.writeByte(uint8_t(player));
	if (sendtoNetMessage(clSocket, output, &sA) <= 0) { return MESSAGE_ERROR; }

	//RECEIVING
	NetworkMessage msg(_INPUT);
	if (recvfromNetMessage(clSocket, msg, &sA) <= 0)
		return MESSAGE_ERROR;
	else
	{
		cur_Seq = msg.readShort();
		char type = msg.readByte();
		switch (type)
		{
		case SV_OKAY:
			state.gamePhase = RUNNING;
			break;
		case SV_FULL:
			return SHUTDOWN;
			break;
		}
	}

	return SUCCESS;
}

// Receive and process messages from the server.
int Client::run()
{
	// TODO: Continuously process messages from the server aslong as the client running. CHECK
	// HINT: Set game phase to DISCONNECTED on SV_CL_CLOSE! (Try calling stop().) CHECK
	// HINT: You can keep track of the number of snapshots with a static variable... CHECK
	while (state.gamePhase == RUNNING)
	{
		//RECEIVING
		NetworkMessage msg(_INPUT);
		if (recvfromNetMessage(clSocket, msg, &sA) <= 0)
			return MESSAGE_ERROR;
		else
		{
			uint16_t temp_Seq;
			temp_Seq = msg.readShort();
			if (temp_Seq > cur_Seq)
			{
				cur_Seq = temp_Seq;
			}

			uint8_t type = msg.readByte();
			uint8_t phase = msg.readByte();

			int16_t ballX = msg.readShort();
			int16_t ballY = msg.readShort();

			int16_t p0_y = msg.readShort();
			int16_t p0_Score = msg.readShort();

			int16_t p1_y = msg.readShort();
			int16_t p1_Score = msg.readShort();
			
			switch (type)
			{
			case SV_SNAPSHOT:
				snapCount++;

				state.gamePhase = phase;
				state.ballX = ballX;
				state.ballY = ballY;
				state.player0.y = p0_y;
				state.player0.score = p0_Score;
				state.player1.y = p1_y;
				state.player1.score = p1_Score;

				if (snapCount % 10 == 0)
				{
					sendAlive();
					snapCount = 0;
				}
				break;
			case CL_KEYS:
				return SUCCESS;
				break;
			case SV_CL_CLOSE:
				stop();
				return SHUTDOWN;
				break;
			}
		}
	}

	return DISCONNECT;
}

// Clean up and shut down the client.
void Client::stop()
{
	// TODO:
	//       1) Make sure to send a SV_CL_CLOSE message. CHECK
	//       2) Make sure to mark the client as shutting down and close the socket. CHECK
	//       3) Set the game phase to DISCONNECTED. CHECK
	NetworkMessage output(_OUTPUT);
	output.writeByte(SV_CL_CLOSE);
	sendtoNetMessage(clSocket, output, &sA);

	shutdown(clSocket, SD_BOTH);
	closesocket(clSocket);
	state.gamePhase = DISCONNECTED;
}

// Send the player's input to the server.
int Client::sendInput(int8_t keyUp, int8_t keyDown, int8_t keyQuit)
{
	if (keyQuit)
	{
		stop();
		return SHUTDOWN;
	}

	cs.enter();
	if (player == 0)
	{
		state.player0.keyUp = keyUp;
		state.player0.keyDown = keyDown;
	}
	else
	{
		state.player1.keyUp = keyUp;
		state.player1.keyDown = keyDown;
	}
	cs.leave();

	//TODO:	Transmit the player's input status.
	NetworkMessage output(_OUTPUT);
	output.writeByte(CL_KEYS);
	output.writeByte(keyUp);
	output.writeByte(keyDown);

	sendtoNetMessage(clSocket, output, &sA);

	return SUCCESS;
}

// Copies the current state into the struct pointed to by target.
void Client::getState(GameState* target)
{
	// TODO: Copy state into target.
	target->gamePhase = state.gamePhase;
	
	target->ballX = state.ballX;
	target->ballY = state.ballY;
	
	target->player0.y = state.player0.y;
	target->player1.y = state.player1.y;

	target->player0.score = state.player0.score;
	target->player1.score = state.player1.score;
}

// Sends a SV_CL_CLOSE message to the server (private, suggested)
void Client::sendClose()
{
	// TODO: Send a CL_CLOSE message to the server.
	NetworkMessage output(_OUTPUT);
	output.writeByte(SV_CL_CLOSE);
	sendtoNetMessage(clSocket, output, &sA);
}

// Sends a CL_ALIVE message to the server (private, suggested)
int Client::sendAlive()
{
	// TODO: Send a CL_ALIVE message to the server.
	NetworkMessage output(_OUTPUT);
	output.writeByte(CL_ALIVE);
	sendtoNetMessage(clSocket, output, &sA);

	return SUCCESS;
}
