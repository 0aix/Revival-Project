#ifndef VOICE_H
#define VOICE_H

#include <Windows.h>

struct IXAudio2;
struct IXAudio2SourceVoice;
struct OggVorbis_File;

class Voice
{
public:
	//small buffer size means faster loading but requires more frequent update calls
	Voice(IXAudio2* IXA2, int buffercount, int buffersize);
	~Voice();
	bool Load(const char* filename);
	bool Play(bool loop);
	void Update();
	void Pause();
	void Stop();
	void SetVolume(float volume);
	float GetVolume();
	int GetSamplesPlayed();

	//get/set variables are ok public
	bool bRunning;
	bool bInUse;
	DWORD sampleRate;

private:
	IXAudio2* pXAudio2;
	IXAudio2SourceVoice* pSourceVoice;
	char** buffers;
	int count;
	DWORD size;
	OggVorbis_File* vf;
	bool bPaused;
	bool bAlmostDone;
	bool bDone;
	bool bLoop;
	int currentBuffer;

};

struct VoiceList
{
	Voice* pVoice = NULL;
	VoiceList* pNext = NULL;

	~VoiceList() { delete pVoice; }
};


#endif