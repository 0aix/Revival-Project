#ifndef AUDIO_H
#define AUDIO_H
#include "Sound.h"
#include "Buffer.h"

namespace Audio
{
	bool Initialize();
	void Uninitialize();
	void Update();

	RAWSOUND* CreateRAWSOUND(BUFFER* buffer);
	RAWSFX* PlayRAWSFX(RAWSOUND* raw);
	Sound* CreateSound(BUFFER* buffer);
	Voice* CreateVoice();
};

#endif