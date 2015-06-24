#ifndef AUDIO_H
#define AUDIO_H
#include <XAudio2.h>

class Voice;
struct OggVorbis_File;
struct BUFFER;

struct RAWSOUND
{
	XAUDIO2_BUFFER buffer;
	WAVEFORMATEX wfm;
};

struct SOUND
{
	OggVorbis_File* vf;
	WAVEFORMATEX wfm;
};

namespace Audio
{
	bool Initialize();
	void Uninitialize();
	void Update();
	Voice* CreateVoice(int buCount = 3, int buSize = 655360);

	RAWSOUND* LoadRawSound(BUFFER* buffer);
	SOUND* LoadSound(BUFFER* buffer);
};

#endif