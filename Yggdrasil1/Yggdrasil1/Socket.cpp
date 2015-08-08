#include "stdafx.h"
#include "Network.h"

//===================Socket===================

Socket::Socket()
{
	handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

Socket::~Socket()
{
	closesocket(handle);
}

bool Socket::Open(unsigned short port)
{
	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(port);

	if (bind(handle, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR_IN)) < 0)
		return false;

	DWORD nonblocking = 1;
	if (ioctlsocket(handle, FIONBIO, &nonblocking) != 0)
		return false;

	return true;
}

void Socket::Close()
{
	closesocket(handle);
}

bool Socket::Send(Address& dest, Packet& packet)
{
	int size = packet.size;
	return sendto(handle, (char*)packet.data, size, 0, (SOCKADDR*)&dest.addr, sizeof(SOCKADDR_IN)) == size;
}

bool Socket::Receive(Address& from, Packet& packet)
{
	byte buffer[MAX_PACKET_SIZE];
	SOCKADDR_IN addr;
	int size = sizeof(SOCKADDR_IN);
	if (recvfrom(handle, (char*)buffer, MAX_PACKET_SIZE, 0, (SOCKADDR*)&addr, &size) > 0)
	{
		from = Address(addr);
		return true;
	}
	return false;
}

//===================Address===================

Address::Address(byte a, byte b, byte c, byte d, unsigned short p)
{
	address = a << 24 | b << 16 | c << 8 | d;
	port = p;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(address);
	addr.sin_port = htons(p);
}

Address::Address(unsigned long a, unsigned short p)
{
	address = a;
	port = p;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(a);
	addr.sin_port = htons(p);
}

Address::Address(SOCKADDR_IN a)
{
	address = ntohl(a.sin_addr.s_addr);
	port = ntohs(a.sin_port);
	addr = a;
}

inline bool Address::operator==(const Address& rhs)
{
	return this->address == rhs.address && this->port == rhs.port;
}