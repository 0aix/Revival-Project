#include "Sound.h"
#include <vorbisfile.h>

Sound::~Sound()
{
	if (pSourceVoice)
	{
		pSourceVoice->Stop();
		pSourceVoice->DestroyVoice();
	}
	delete pSourceVoice;
	if (vf)
		ov_clear(vf);
	delete vf;
}

void Sound::Start()
{

}

void Sound::Stop()
{

}