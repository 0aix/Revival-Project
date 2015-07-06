#include "stdafx.h"
#include "Sound.h"

Voice::~Voice()
{
	SoundList* curr = pSoundFirst;
	SoundList* temp;
	while (curr != NULL)
	{
		temp = curr;
		curr = curr->pNext;
		delete temp;
	}
}

void Voice::Queue(Sound* pSound)
{
	if (bExit)
		return;

	mtx.lock();

	if (pSoundFirst == NULL)
	{
		pSoundFirst = new SoundList;
		pSoundFirst->pData = pSound;
		pSoundLast = pSoundFirst;
	}
	else //assert pSoundLast != NULL
	{
		SoundList* temp = new SoundList;
		temp->pData = pSound;
		pSoundLast->pNext = temp;
		pSoundLast = temp;
	}

	mtx.unlock();
}

void Voice::Next()
{
	if (bExit)
		return;

	mtx.lock();

	if (bLoop)
	{
		pSoundLast->pNext = pSoundFirst;
		pSoundLast = pSoundFirst;
		pSoundFirst = pSoundFirst->pNext;
		pSoundLast->pNext = NULL;
	}
	else
	{
		SoundList* temp = pSoundFirst->pNext;
		delete pSoundFirst;
		pSoundFirst = temp;
	}

	mtx.unlock();
}

void Voice::Start()
{
	if (bRunning || bExit)
		return;

	mtx.lock();

	if (!pSoundFirst)
		return;
	if (FAILED(pXAudio2->CreateSourceVoice(&pSourceVoice, &pSoundFirst->pData->wfm, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this, NULL, NULL)))
		return;
	if (FAILED(pSourceVoice->Start()))
	{
		pSourceVoice->DestroyVoice();
		mtx.unlock();
		return;
	}
	Stream();
	bDone = false;
	bPaused = false;
	bRunning = true;

	mtx.unlock();
}

void Voice::Stream()
{
	if (!bRunning || bDone || bExit)
		return;

	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);
	for (int i = state.BuffersQueued; i < VOICE_BUFFER_COUNT; i++)
	{
		OggVorbis_File* vf = pSoundFirst->pData->vf;

		memset(buffers[currentBuffer], 0, VOICE_BUFFER_SIZE);

		DWORD pos = 0;
		int sec = 0;
		int ret = 1;

		while (ret > 0 && pos < VOICE_BUFFER_SIZE)
		{
			ret = ov_read(vf, (char*)buffers[currentBuffer] + pos, VOICE_BUFFER_SIZE - pos, 0, 2, 1, &sec);
			pos += ret;
		}

		XAUDIO2_BUFFER buffer = { 0 };
		buffer.pAudioData = (BYTE*)buffers[currentBuffer];
		buffer.AudioBytes = VOICE_BUFFER_SIZE;
		if (ret == 0) //end of vorbis file stream
		{
			ov_pcm_seek(vf, 0);
			buffer.Flags = XAUDIO2_END_OF_STREAM;
			bDone = true;
		}

		if (FAILED(pSourceVoice->SubmitSourceBuffer(&buffer)))
			return;

		currentBuffer++;
		currentBuffer %= VOICE_BUFFER_COUNT;
	}
}

void Voice::Pause()
{
	if (!bRunning || bExit)
		return;

	mtx.lock();

	if (bPaused)
		pSourceVoice->Start();
	else
		pSourceVoice->Stop();

	bPaused = !bPaused;

	mtx.unlock();
}

void Voice::Stop()
{
	if (!bRunning || bExit)
		return;

	mtx.lock();

	pSourceVoice->Stop();
	pSourceVoice->DestroyVoice();
	bRunning = false;

	mtx.unlock();
}

void Voice::Loop()
{
	mtx.lock();

	bLoop = !bLoop;

	mtx.unlock();
}

void Voice::Destroy()
{
	if (bExit)
		return;

	Stop();
	bExit = true;
}

void Voice::SetVolume(float volume)
{
	pSourceVoice->SetVolume(volume);
}

float Voice::GetVolume()
{
	if (!pSourceVoice)
		return 0.0f;

	float volume;
	return pSourceVoice->GetVolume(&volume), volume;
}