#ifndef AUDIO_H
#define AUDIO_H

struct BUFFER;
struct RAWSOUND;
struct RAWSFX;
class Sound;
class Voice;

namespace Audio
{
	bool Initialize();
	void Uninitialize();
	void Update();

	Voice* CreateVoice(int buCount, int buSize);

	RAWSOUND* CreateRAWSOUND(BUFFER* buffer);
	RAWSFX* CreateRAWSFX(RAWSOUND* raw);

	Sound* CreateSound(BUFFER* buffer, bool loop);
};

#endif