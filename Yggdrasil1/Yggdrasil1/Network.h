#pragma once

#include "Misc.h"

#define MAX_PACKET_SIZE 1024

class ISocketHandler;
class ICharacter;
class World;

typedef void(*Func)(ISocketHandler* handler);

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

//add read/write
struct Packet
{
	byte* data;
	unsigned long length;
	unsigned long maxlength;
	unsigned long index = 0;

	Packet();
	Packet(unsigned long size);
	~Packet();

	bool ReadyToRead();
	bool ReadBytes(byte* dest, unsigned long len);

	template <typename T>
	bool Read(T& get)
	{
		unsigned long size = sizeof(T);
		if (index + size <= length)
		{
			get = *(T*)(data + index);
			index += size;
			return true;
		}
		return false;
	}

	bool ReadyToWrite();
	bool WriteBytes(byte* src, unsigned long len);
	template <typename T>
	bool Write(T set)
	{
		unsigned long size = sizeof(T);
		if (index + size <= maxlength)
		{
			*(T*)(data + index) = set;
			index += size;
			length += size;
			return true;
		}
		return false;
	}
};

struct PacketInfo
{
	byte* data;
	unsigned long length = 0; //length 0 means do not resend
	byte uid; //unique id for msg type
	double time_sent;
	
	PacketInfo();
	PacketInfo(Packet& packet);
	~PacketInfo();
};

struct PacketHandler
{
	Address addr;
	Packet recv;
	Packet send;
	UINT16 recv_id; //seq number for recv packet
};

struct Ack
{
	UINT16 recv_id = 0; //most recent recv packet seq
	UINT16 send_id = 0; //most recent send packet seq
	UINT64 recv_ack = 0; //ack bitfield - lowest bit is most recent
	UINT64 send_ack = 0;

	//2 bytes : 0 - 65535
	static bool seq_recent(UINT16 s1, UINT16 s2)
	{
		return (s1 > s2 && s1 - s2 <= 32768) || (s2 > s1 && s2 - s1  > 32768);
	}
};

//class IConnection
struct Connection
{
	Address addr;
	Ack ack;
	DWORD hi;
	DWORD lo;
	ICharacter* player; //World releases the character for now

	bool connected = false;
};

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

class ISocketHandler
{
public:
	ISocketHandler(int count);
	~ISocketHandler();
	virtual void Receive() = 0;
	virtual bool Handle() = 0;
	virtual void Update() = 0;
	virtual void Broadcast() = 0;

protected:
	Socket socket;
	PacketHandler packet;

	Func* funcTable;
	int tableCount;
};

class ServerHandler : public ISocketHandler
{
public:
	ServerHandler(int maxconn);
	~ServerHandler();
	void Receive();
	bool Handle();
	void Update();
	void Broadcast();

	World* world;

private:
	static const byte header[4];

	Connection** client;
	int maxCount;
	int playerCount;

	//Func Table
	static void Connect(ISocketHandler* handler);
};