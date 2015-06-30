#include "Buffer.h"

ov_callbacks BUFFER::OV_CALLBACKS_BUFFER =
{
	(size_t(*)(void*, size_t, size_t, void*))read,
	(int(*)(void*, ogg_int64_t, int))seek,
	//NULL, //no close
	(int(*)(void*))close,
	(long(*)(void*))tell
};

size_t BUFFER::read(void* ptr, size_t size, size_t count, BUFFER* buffer)
{
	if (!buffer)
		return 0;

	size_t total = size * count;
	size_t range = buffer->dwSize - buffer->dwPos;
	if (total > range)
	{
		count = range / size;
		total = size * count;
	}
	memcpy(ptr, buffer->pBase + buffer->dwPos, total);
	buffer->dwPos += total;
	return count;
}

int BUFFER::seek(BUFFER* buffer, ogg_int64_t offset, int origin)
{
	if (!buffer)
		return -1;
	if (origin == SEEK_SET)
	{
		if (offset < 0)
			return -1;
		if (offset > buffer->dwSize)
			return -1;
		buffer->dwPos = offset;
	}
	else if (origin == SEEK_CUR)
	{
		DWORD pos = buffer->dwPos + offset;
		if (pos > buffer->dwSize)
			return -1;
		buffer->dwPos = pos;
	}
	else if (origin == SEEK_END)
		buffer->dwPos = buffer->dwSize;
	else
		return -1;
	return 0;
}

int BUFFER::close(BUFFER* buffer)
{
	if (!buffer)
		return -1;

	delete[] buffer->pBase;
	delete buffer;
	return 0;
}

long BUFFER::tell(BUFFER* buffer)
{
	if (!buffer)
		return -1;
	return buffer->dwPos;
}