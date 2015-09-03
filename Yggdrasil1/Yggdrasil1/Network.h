#pragma once

#include "Misc.h"

#define MAX_PACKET_SIZE 1024
#define MAX_DATA_SIZE 256

class ISocketHandler;
class ICharacter;
class World;

typedef void(*Func)(ISocketHandler* handler);

struct Address
{
	DWORD address;
	WORD port;
	SOCKADDR_IN addr;

	Address() {}
	Address(BYTE a, BYTE b, BYTE c, BYTE d, WORD p);
	Address(DWORD a, WORD p);
	Address(SOCKADDR_IN a);
	inline bool operator==(const Address& rhs);
};

//Packets will never be pointers. always placed on the stack. use by reference
struct Packet
{
	BYTE* data;
	WORD length;
	WORD maxlength;
	WORD index = 0;
	bool canFlush = true;

	Packet(WORD size = MAX_PACKET_SIZE);
	~Packet();

	bool ReadyToRead();
	bool ReadBytes(BYTE* dest, WORD len);

	template <typename T>
	bool Read(T& get)
	{
		WORD size = sizeof(T);
		if (index + size <= length)
		{
			get = *(T*)(data + index);
			index += size;
			return true;
		}
		return false;
	}

	bool ReadyToWrite();
	bool WriteBytes(BYTE* src, WORD len);
	template <typename T>
	bool Write(T set)
	{
		WORD size = sizeof(T);
		if (index + size <= maxlength)
		{
			*(T*)(data + index) = set;
			length += size;
			return true;
		}
		canFlush = false;
		return false;
	}
	bool Flush(bool flush = true);
};

//exception to the packets. always use Data as pointers
struct Data : Packet
{
	bool critical;

	Data(WORD size, bool c);
};

typedef LList<Data> DataList;

struct PacketRecord
{
	DataList dataList;
	double time_sent;

	~PacketRecord();
};

struct PacketRecordHandler
{
	PacketRecord record[32];
	WORD id = 0; //most recent packet seq
	BYTE left = 0;
	BYTE right = 0; //corresponds with id
};

struct Ack
{
	WORD id = 0; //most recent packet seq
	DWORD bitfield = 0; //ack bitfield - lowest bit is most recent

	//returns xor
	DWORD UpdateAck(WORD seq, DWORD bits)
	{
		if (seq_recent(seq, id))
		{
			WORD diff = seq - id;
			if (diff >= 32)
			{
				bitfield = bits;
				return bits;
			}

		}
		else
		{

		}
	}

	//2 bytes : 0 - 65535
	//true if s1 more recent than s2
	static bool seq_recent(WORD s1, WORD s2)
	{
		return (s1 > s2 && s1 - s2 <= 32768) || (s2 > s1 && s2 - s1  > 32768);
	}
};

//class IConnection
struct Connection
{
	Address addr;
	//PacketRecordHandler handler;
	DataList dataList;
	Ack recv_ack;
	Ack send_ack;
	DWORD hi;
	DWORD lo;
	ICharacter* player; //World releases the character for now

	bool connected = false; //??
	bool ready = false;
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
	ISocketHandler(BYTE count);
	~ISocketHandler();
	virtual void Ready() = 0;
	virtual void Receive() = 0;
	virtual bool Handle() = 0;
	virtual void Update() = 0;
	virtual void Broadcast() = 0;

	virtual bool Send() = 0;

protected:
	Socket socket;
	Address addr;
	Packet send;
	Packet recv;
	WORD recv_id; //seq number for recv packet

	Func* funcTable;
	BYTE tableCount;
};

class ServerHandler : public ISocketHandler
{
public:
	ServerHandler(int maxconn);
	~ServerHandler();
	void Ready();
	void Receive();
	bool Handle();
	void Update();
	void Broadcast();

	bool Send();

	World* world;

private:
	static const byte header[4];

	Connection** client;
	int maxCount;
	int playerCount;
	Clock clock;

	//Func Table
	static void Connect(ISocketHandler* handler);
	static void Ack(ISocketHandler* handler);
};