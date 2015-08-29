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
	client = new Connection*[maxconn];

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
	byte data[4];
	while (socket.Receive(packet.addr, packet.recv))
		if (packet.recv.ReadBytes(data, 4)
			&& memcmp(header, data, 4)
			&& packet.recv.Read<UINT16>(packet.recv_id)) //"TRPH__" has length 6
			while (Handle());
}

bool ServerHandler::Handle()
{
	//Get packet category
	byte func;
	if (!packet.recv.Read<byte>(func))
		return false;

	//Check if address is recognized
	unsigned long ip = packet.addr.address;
	unsigned short port = packet.addr.port;
	bool exists = false;
	for (int i = 0; i < maxCount; i++)
	{
		Connection* user = client[i];
		if (user != NULL && ip == user->addr.address && port == user->addr.port)
		{
			exists = true;
			break;
		}
	}

	//Either it exists and it's not a connection request or vice-versa
	//Check if funcTable[func] exists
	if (exists ^ (func == 0x00) && func < tableCount && funcTable[func] != NULL)
	{
		funcTable[func](this);
		return true;
	}
	return false;
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
	PacketHandler packet = server->packet;
	byte data[8];
	if (packet.send.ReadyToWrite())
	{
		packet.send.WriteBytes((byte*)header, 4);
		if (!packet.recv.ReadBytes(data, 8)) //'TRPH__' 00 - No extra data; simple connection request
		{
			packet.send.Write<UINT16>(0x0000); //packet send seq
			packet.send.Write<byte>(0x00); //packet type - connection
			int maxCount = server->maxCount;
			if (server->playerCount < maxCount)
			{
				//8 bytes used for some requests
				DWORD hi = (DWORD)rand() * packet.addr.address;
				DWORD lo = (DWORD)rand() * packet.addr.port + rand();

				//'TRPH' 00 00 00 ? ? ? ? ? ? ? ?
				packet.send.Write<DWORD>(lo);
				packet.send.Write<DWORD>(hi);

				//Look for vacant connection (has to be > 0)
				Connection** client = server->client;
				int i = 0;
				for (; i < maxCount; i++)
					if (!client[i])
						break;

				//Add player to server / character to world
				Connection* user = new Connection;
				user->addr = packet.addr;
				user->hi = hi;
				user->lo = lo;
				user->player = new Shamoo;

				client[i] = user;
				server->world->players[i] = user->player;
				server->playerCount++;
			}
			//Denied connection looks like 'TRPH__' 00
			server->socket.Send(packet.addr, packet.send);
		}
	}
	else //Check for reconnection request : 8 byte identifier
	{
		
	}
}