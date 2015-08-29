#include "stdafx.h"
#include "Network.h"

Packet::Packet()
{
	data = new byte[MAX_PACKET_SIZE];
	length = MAX_PACKET_SIZE;
	maxlength = MAX_PACKET_SIZE;
}

Packet::Packet(unsigned long size)
{
	data = new byte[size];
	length = size;
	maxlength = size;
}

Packet::~Packet() 
{
	delete[] data;
};

bool Packet::ReadyToRead()
{
	index = 0;
	return data != NULL && length > 0;
}

bool Packet::ReadBytes(byte* dest, unsigned long len)
{
	if (index + len <= length)
	{
		memcpy(dest, data + index, len);
		index += len;
		return true;
	}
	return false;
}

bool Packet::ReadyToWrite()
{
	index = 0;
	length = 0;
	return data != NULL;
}

bool Packet::WriteBytes(byte* src, unsigned long len)
{
	if (index + len <= maxlength)
	{
		memcpy(data + index, src, len);
		index += len;
		length += len;
		return true;
	}
	return false;
}