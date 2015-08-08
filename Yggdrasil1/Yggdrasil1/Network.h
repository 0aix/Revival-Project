#pragma once

#define MAX_PACKET_SIZE 256

struct Address;
class IConnection;
struct Packet;

typedef void(*Func)(IConnection* conn, Packet* packet);

class Socket
{
public:
	Socket();
	~Socket();
	bool Open(unsigned short port);
	void Close();
	bool Send(Address& dest, Packet& packet);
	bool Receive(Address& from, Packet& packet);

private:
	SOCKET handle;
};

struct Address
{
	unsigned long address;
	unsigned short port;
	SOCKADDR_IN addr;

	Address();
	Address(byte a, byte b, byte c, byte d, unsigned short p);
	Address(unsigned long a, unsigned short p);
	Address(SOCKADDR_IN a);
	inline bool operator==(const Address& rhs);
};

struct Packet
{
	void* data;
	int size;
};

class IPacketHandler
{
public:
	IPacketHandler(int size);
	~IPacketHandler();

private:
	Func* funcTable;
	int tableCount;
};

class ServerHandler : public IPacketHandler
{
public:
	ServerHandler(int size);
	~ServerHandler();

private:

};

class ISocketHandler
{
	Socket socket;
	IPacketHandler* handler;
};

class IConnection
{
	Address addr;
};