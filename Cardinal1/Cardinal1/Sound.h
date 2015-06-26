#ifndef SOUND_H
#define SOUND_H
#include <XAudio2.h>

#define BUFFER_COUNT 3
#define BUFFER_SIZE 65536

struct OggVorbis_File;

struct RAWSOUND
{
	XAUDIO2_BUFFER buffer;
	WAVEFORMATEX wfm;
};

class RAWSFX : public IXAudio2VoiceCallback
{
public:
	IXAudio2SourceVoice* pSourceVoice = NULL;
	bool bDone = false;

	void Kill()
	{
		if (pSourceVoice)
		{
			pSourceVoice->Stop();
			pSourceVoice->DestroyVoice();
		}
		delete pSourceVoice;
		bDone = true;
	}

	//IXAudio2VoiceCallback
	void OnStreamEnd() { Kill(); }

	void OnVoiceProcessingPassEnd() {}
	void OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
	void OnBufferEnd(void* pBufferContext) {}
	void OnBufferStart(void* pBufferContext) {}
	void OnLoopEnd(void* pBufferContext) {}
	void OnVoiceError(void* pBufferContext, HRESULT Error) {}
};

struct RAWSFXList
{
	RAWSFX* pRAWSFX = NULL;
	RAWSFXList* pNext = NULL;
};

class Sound
{
public:
	OggVorbis_File* vf;
	WAVEFORMATEX wfm;
	IXAudio2SourceVoice* pSourceVoice = NULL;
	BYTE buffers[BUFFER_COUNT][BUFFER_SIZE];

	Sound(bool loop) { bLoop = loop; }
	~Sound();

	void Start();
	void Stop();

private:
	bool bLoop = false;
};

struct SoundList
{
	Sound* pSound = NULL;
	SoundList* pNext = NULL;
};

#endif