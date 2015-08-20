#pragma once

#define MAX_PACKET_SIZE 1024

struct Address;
struct Packet;
class ISocketHandler;
class ICharacter;
class World;

typedef void(*Func)(ISocketHandler* handler);

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

	Address() {}
	Address(byte a, byte b, byte c, byte d, unsigned short p);
	Address(unsigned long a, unsigned short p);
	Address(SOCKADDR_IN a);
	inline bool operator==(const Address& rhs);
};

struct Packet
{
	byte* data;
	int length;
	int index = 0;

	Packet(int size)
	{
		data = new byte[size];
		length = size;
	}
	~Packet() { delete[] data; };
};

struct Client
{
	Address addr;
	UINT64 id;
	ICharacter* player; //World releases the character for now
};

class ISocketHandler
{
public:
	ISocketHandler(int count);
	~ISocketHandler();
	virtual void Receive() = 0;
	virtual void Handle() = 0;
	virtual void Update() = 0;
	virtual void Broadcast() = 0;

protected:
	Socket socket;

	Address from;
	Packet packet = Packet(MAX_PACKET_SIZE);
	WORD packetID;

	Func* funcTable;
	int tableCount;
};

class ServerHandler : public ISocketHandler
{
public:
	ServerHandler(int maxconn);
	~ServerHandler();
	void Receive();
	void Handle();
	void Update();
	void Broadcast();

	World* world;

private:
	static const byte header[4];

	Client** client;
	int maxCount;
	int playerCount;

	//Func Table
	static void Connect(ISocketHandler* handler);
};