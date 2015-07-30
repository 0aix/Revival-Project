#include "stdafx.h"
#include "Sound.h"

void Voice::Queue(Sound* pSound)
{
	if (bExit)
		return;

	soundList.Queue(pSound);
}

void Voice::Next()
{
	if (bLoop)
	{
		Sound* pSound = soundList.At();
		soundList.Remove(false);
		soundList.Queue(pSound);
	}
	else 
		soundList.Remove(false);
}

void Voice::Start()
{
	if (bRunning)
		return;

	Sound* pSound = soundList.At();

	if (!pSound)
		return;
	if (FAILED(pXAudio2->CreateSourceVoice(&pSourceVoice, &pSound->wfm, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this, NULL, NULL)))
		return;
	if (FAILED(pSourceVoice->Start()))
	{
		pSourceVoice->DestroyVoice();
		return;
	}

	Stream();
	bPaused = false;
	bRunning = true;
}

void Voice::Stream()
{
	if (!bRunning || bExit || !pSourceVoice)
		return;

	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);
	for (int i = state.BuffersQueued; i < VOICE_BUFFER_COUNT; i++)
	{
		OggVorbis_File* vf = soundList.At()->vf;

		memset(buffers[currentBuffer], 0, VOICE_BUFFER_SIZE);

		DWORD pos = 0;
		int sec = 0;
		int ret = 1;

		while (ret > 0 && pos < VOICE_BUFFER_SIZE)
		{
			ret = ov_read(vf, (char*)buffers[currentBuffer] + pos, VOICE_BUFFER_SIZE - pos, 0, 2, 1, &sec);
			pos += ret;
		}

		XAUDIO2_BUFFER buffer = { };
		buffer.pAudioData = (BYTE*)buffers[currentBuffer];
		buffer.AudioBytes = VOICE_BUFFER_SIZE;
		if (ret == 0) //end of vorbis file stream
		{
			ov_pcm_seek(vf, 0);
			buffer.Flags = XAUDIO2_END_OF_STREAM;
		}

		if (FAILED(pSourceVoice->SubmitSourceBuffer(&buffer)))
			return;

		currentBuffer++;
		currentBuffer %= VOICE_BUFFER_COUNT;
	}
}

void Voice::Pause()
{
	if (!pSourceVoice)
		return;

	if (bPaused)
		pSourceVoice->Start();
	else
		pSourceVoice->Stop();

	bPaused = !bPaused;
}

void Voice::Stop()
{
	if (!pSourceVoice)
		return;

	pSourceVoice->Stop();
	pSourceVoice->DestroyVoice();
	bRunning = false;
}

void Voice::Loop()
{
	bLoop = !bLoop;
}

void Voice::Exit()
{
	if (bExit)
		return;

	Stop();
	bExit = true;
}

void Voice::SetVolume(float volume)
{
	if (!pSourceVoice)
		return;

	pSourceVoice->SetVolume(volume);
}

float Voice::GetVolume()
{
	if (!pSourceVoice)
		return 0.0f;

	float volume;
	return pSourceVoice->GetVolume(&volume), volume;
}