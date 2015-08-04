#pragma once

#include "Sound.h"

namespace Audio
{
	bool Initialize();
	void Uninitialize();
	void Update();

	RAWSOUND* CreateRAWSOUND(BUFFER* buffer);
	RAWSFX* PlayRAWSFX(RAWSOUND* raw);
	Sound* CreateSound(BUFFER* buffer);
	Voice* CreateVoice();

	size_t read(void* ptr, size_t size, size_t count, BUFFER* buffer);
	int seek(BUFFER* buffer, ogg_int64_t offset, int origin);
	int close(BUFFER* buffer);
	long tell(BUFFER* buffer);
};