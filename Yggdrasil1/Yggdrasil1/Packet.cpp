#include "stdafx.h"
#include "Network.h"

Packet::Packet(WORD size)
{
	if (size > MAX_PACKET_SIZE)
		size = MAX_PACKET_SIZE;
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

bool Packet::ReadBytes(byte* dest, WORD len)
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
	canFlush = true;
	return data != NULL;
}

bool Packet::WriteBytes(byte* src, WORD len)
{
	if (index + len <= maxlength)
	{
		memcpy(data + index, src, len);
		index += len;
		length += len;
		return true;
	}
	canFlush = false;
	return false;
}

bool Packet::Flush(bool flush)
{
	if (flush && canFlush)
	{
		length = index;
		return true;
	}
	else
	{
		index = length;
		canFlush = true;
		return !flush;
	}
}

//========================Data========================

Data::Data(WORD size, bool c) : Packet(size < MAX_DATA_SIZE ? size : MAX_DATA_SIZE)
{
	critical = c;
}

//====================PacketRecord====================

PacketRecord::~PacketRecord()
{
	dataList.RemoveAll();
}