#pragma once

class Socket
{
public:
	Socket();
	~Socket();
	bool Open(unsigned short port);
	void Close();
	bool Send(Address& dest, void* data, int size);
	int Receive(Address& from, void* data, int size);

private:
	SOCKET handle;
};

struct Address
{
	unsigned long address;
	unsigned short port;

	Address();
	Address(unsigned long a, unsigned short p)
	{
		address = a;
		port = p;
	}
};