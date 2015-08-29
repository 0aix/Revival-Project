#include "stdafx.h"
#include "Network.h"
#include "World.h"

#include <stdio.h>

//world w/ 2 players

bool running = true;
const double tickrate = 60.0; //per second
const double tick = 1.0 / tickrate;
GameClock clock;
double time = 0.0;
ServerHandler server(2);

void loop()
{
	clock.Start();
	double time = clock.Time();

	//main loop
	while (running)
	{
		time += clock.Time();
		while (time >= C::TICK)
			server.Update();
		Sleep(0); //people say dont put this here but...
	}
}

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;
	printf("WSA OK\n");

	//Start a basic server loop with one world
	HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)loop, NULL, 0, NULL);
	if (!thread)
		return 0;
	printf("Server thread started\n");
	
	//exit
	getchar();
	printf("Exiting\n");
	running = false;
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);

	WSACleanup();
	return 0;
}

/*Socket socket;
socket.Open(SERVER_PORT);

Address to = Address(127, 0, 0, 1, SERVER_PORT);
char str[] = "hey what's up";
Packet packet;
packet.data = str;
packet.size = strlen(str);
socket.Send(to, packet);

socket.Close();*/