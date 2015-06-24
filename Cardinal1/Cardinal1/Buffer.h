#ifndef BUFFER_H
#define BUFFER_H
#include "Main.h"
#include <vorbisfile.h>

struct BUFFER
{
	BYTE* pBase = NULL;
	DWORD dwSize = 0;
	DWORD dwPos = 0;

	BUFFER() {}
	BUFFER(DWORD length)
	{
		pBase = new BYTE[length];
		dwSize = length;
	}
	//pBase should be deleted manually

	static ov_callbacks OV_CALLBACKS_BUFFER;
	static size_t read(void* ptr, size_t size, size_t count, BUFFER* buffer);
	static int seek(BUFFER* buffer, ogg_int64_t offset, int origin);
	static int close(BUFFER* buffer);
	static long tell(BUFFER* buffer);
};

#endif