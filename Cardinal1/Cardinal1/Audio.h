#ifndef AUDIO_H
#define AUDIO_H

class Voice;
struct IXAudio2;
struct IXAudio2MasteringVoice;
struct VoiceList;

namespace Audio
{
	bool Initialize();
	void Uninitialize();
	void Update();
	Voice* CreateVoice(int buCount = 3, int buSize = 655360);
};

#endif