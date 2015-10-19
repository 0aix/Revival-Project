#include "stdafx.h"
#include "Network.h"
#include "World.h"

#define SERVER_PORT 17460

//=============ISocketHandler=============

ISocketHandler::ISocketHandler(byte count)
{
	//Always leave 1 spot for connection request
	tableCount = count + 1;
	funcTable = new Func[tableCount];
}

ISocketHandler::~ISocketHandler()
{
	delete[] funcTable;
}

//=============ServerHandler=============

const byte ServerHandler::header[4] = { 'T', 'R', 'P', 'H' };

ServerHandler::ServerHandler(int maxconn) : ISocketHandler(0xFF)
{
	client = new Connection*[maxconn];

	world = new World(16, 12, maxconn);
	maxCount = maxconn;
	playerCount = 0;
	
	//Populate func table
	funcTable[0x00] = Connect;
	funcTable[0xFF] = Ack;

	socket.Open(SERVER_PORT);
}

ServerHandler::~ServerHandler()
{
	delete world;

	for (int i = 0; i < maxCount; i++)
		delete client[i];
	delete[] client;
}

void ServerHandler::Ready()
{
	
}

void ServerHandler::Receive()
{
	BYTE data[4];
	while (socket.Receive(addr, recv))
		if (recv.ReadBytes(data, 4) &&
			memcmp(header, data, 4) == 0 &&
			recv.Read<WORD>(recv_id)) //"TRPH__" has length 6
			while (Handle());
	
}

bool ServerHandler::Handle()
{
	//Get packet type
	BYTE func;
	if (!recv.Read<BYTE>(func))
		return false;

	//Check if address is recognized
	unsigned long ip = addr.address;
	unsigned short port = addr.port;
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

bool ServerHandler::Send()
{
	if (socket.Send(addr, send))
	{
		//do ack stuff
		//

		return true;
	}
	return false;
}

void ServerHandler::Connect(ISocketHandler* handler)
{
	ServerHandler* server = (ServerHandler*)handler;
	Packet& recv = server->recv;
	byte buffer[8];
	if (!recv.ReadBytes(buffer, 8)) //No extra data; simple connection request
	{
		int maxCount = server->maxCount;
		if (server->playerCount < maxCount)
		{
			Data* data = new Data(9, true);
			data->Write<BYTE>(0x00); //packet type - connection

			//8 bytes used for authorized requests
			DWORD hi = (DWORD)rand() * server->addr.address;
			DWORD lo = (DWORD)rand() * server->addr.port + rand();

			//00 ? ? ? ? ? ? ? ?
			data->Write<DWORD>(lo);
			data->Write<DWORD>(hi);
			data->Flush();

			//Look for vacant connection
			Connection** client = server->client;
			int i = 0;
			for (; i < maxCount; i++)
				if (!client[i])
					break;

			//Add player to server / character to world
			Connection* user = new Connection;
			user->addr = server->addr;
			user->hi = hi;
			user->lo = lo;
			user->player = new Shamoo;
			user->dataList.Queue(data);

			client[i] = user;
			server->world->players[i] = user->player;
			server->playerCount++;
		}
		else //Denied connection looks like 'TRPH__' 00
		{
			Packet& send = server->send;
			send.ReadyToWrite();
			send.WriteBytes((BYTE*)server->header, 4);
			send.Write<BYTE>(0x00);
			server->socket.Send(server->addr, send);
		}
	}
	else //Check for reconnection request : 8 byte identifier
	{
		
	}
}

void ServerHandler::Ack(ISocketHandler* handler)
{
	ServerHandler* server = (ServerHandler*)handler;
}