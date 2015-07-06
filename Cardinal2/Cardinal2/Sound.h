#pragma once

#include <XAudio2.h>
#include <mutex>
#include <vorbisfile.h>
#include "Misc.h"

#define VOICE_BUFFER_COUNT 3
#define VOICE_BUFFER_SIZE 65536

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
	void __stdcall OnStreamEnd() { Kill(); }

	void __stdcall OnVoiceProcessingPassEnd() {}
	void __stdcall OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
	void __stdcall OnBufferEnd(void* pBufferContext) {}
	void __stdcall OnBufferStart(void* pBufferContext) {}
	void __stdcall OnLoopEnd(void* pBufferContext) {}
	void __stdcall OnVoiceError(void* pBufferContext, HRESULT Error) {}
};

typedef LList<RAWSFX> RAWSFXList;

struct Sound
{
	OggVorbis_File* vf = NULL;
	WAVEFORMATEX wfm;

	~Sound()
	{
		if (!vf)
			ov_clear(vf);
	}
};

typedef LList<Sound> SoundList;

class Voice : public IXAudio2VoiceCallback //ver 1 no error-checking
{
public:
	Voice(IXAudio2* pAudio) { pXAudio2 = pAudio; }
	~Voice();
	void Start();
	void Pause();
	void Stop();
	void Loop();

	void Queue(Sound* pSound);
	void Next();
	void Stream();
	void Destroy();

	void SetVolume(float volume);
	float GetVolume();

	//IXAudio2VoiceCallback
	void __stdcall OnStreamEnd()
	{
		Stop();
		Next();
		Start();
	}

	void __stdcall OnVoiceProcessingPassEnd() {}
	void __stdcall OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
	void __stdcall OnBufferEnd(void* pBufferContext) {}
	void __stdcall OnBufferStart(void* pBufferContext) {}
	void __stdcall OnLoopEnd(void* pBufferContext) {}
	void __stdcall OnVoiceError(void* pBufferContext, HRESULT Error) {}

	//
	IXAudio2SourceVoice* pSourceVoice = NULL;
	bool bPaused = false;
	bool bRunning = false;
	bool bExit = false;

private:
	std::mutex mtx;
	IXAudio2* pXAudio2;
	SoundList* pSoundFirst = NULL;
	SoundList* pSoundLast = NULL;
	BYTE buffers[VOICE_BUFFER_COUNT][VOICE_BUFFER_SIZE];
	BYTE currentBuffer = 0;
	bool bDone = false;
	bool bLoop = false;

};

typedef LList<Voice> VoiceList;