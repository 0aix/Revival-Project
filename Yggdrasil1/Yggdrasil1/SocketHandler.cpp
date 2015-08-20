#include "stdafx.h"
#include "Network.h"
#include "World.h"

#define SERVER_PORT 17460

//=============ISocketHandler=============

ISocketHandler::ISocketHandler(int count)
{
	funcTable = new Func[count];
	tableCount = count;
}

ISocketHandler::~ISocketHandler()
{
	delete[] funcTable;
}

//=============ServerHandler=============

const byte ServerHandler::header[4] = { 'T', 'R', 'P', 'H' };

ServerHandler::ServerHandler(int maxconn) : ISocketHandler(1)
{
	client = new Client*[maxconn];

	world = new World(16, 12, maxconn);
	maxCount = maxconn;
	playerCount = 0;
	
	//Populate func table
	funcTable[0x00] = Connect;

	socket.Open(SERVER_PORT);
}

ServerHandler::~ServerHandler()
{
	delete world;

	for (int i = 0; i < maxCount; i++)
		delete client[i];
	delete[] client;
}

void ServerHandler::Receive()
{
	byte* data = packet.data;
	while (socket.Receive(from, packet))
	{
		int length = packet.length;
		if (length > 6 && memcmp(header, data, 4) == 0) //"TRPH__" has length 6
		{
			packetID = *(WORD*)(data + 4);
			packet.index = 6;
			while (packet.index < length);
				Handle();
		}
	}
}

void ServerHandler::Handle()
{
	//Get packet category
	byte func = *(packet.data + packet.index);

	//Check if address is recognized
	unsigned long ip = from.address;
	unsigned short port = from.port;
	bool exists = false;
	for (int i = 0; i < maxCount; i++)
	{
		Client* user = client[i];
		if (user != NULL)
		{
			if (ip == user->addr.address && port == user->addr.port)
			{
				exists = true;
				break;
			}
		}
	}

	if (!exists && func != 0x00) //If the connection doesn't exist, the packet must be a connection request
		return;

	packet.index++;
	if (func < tableCount)
		funcTable[func](this);
}

void ServerHandler::Update()
{
	Receive();
	world->Update();
	Broadcast();
}

void ServerHandler::Broadcast()
{

}

void ServerHandler::Connect(ISocketHandler* handler)
{
	ServerHandler* server = (ServerHandler*)handler;
	Packet packet = server->packet;
	Address addr = server->from;
	byte* data = packet.data + packet.index;
	int length = packet.length - packet.index;
	if (length == 0) //'TRPH' 00 - No extra data; simple connection request
	{
		//Set-up packet
		Packet response(15);
		byte* buffer = response.data;
		memcpy(buffer, header, 4);
		memset(buffer + 3, 0, 3);
		int max = server->maxCount;
		if (server->playerCount < max)
		{
			UINT64 id = UINT64((DWORD)rand() * addr.address) << 32 & ((DWORD)rand() * addr.port) + rand(); //used for reconnection req.

			//'TRPH' 00 00 00 ? ? ? ? ? ? ? ?
			*(UINT64*)(buffer + 7) = id;

			//Add player to server
			Client* user = new Client();
			user->addr = addr;
			user->id = id;
			user->player = new Shamoo();

			Client** client = server->client;
			int i = 0;
			for (; i < max; i++)
				if (!client[i])
					break;

			client[i] = user;
			server->world->players[i] = user->player;
			server->playerCount++;
		}
		else //Deny connection (no identifier) - 'TRPH' 00 00 00
			response.length = 7;

		server->socket.Send(addr, response);
	}
	else if (length == 8)//if length == 8, check for reconnection request : 8 byte identifier
	{
		
	}
}