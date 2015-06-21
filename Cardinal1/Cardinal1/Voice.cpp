#include "Voice.h"
#include <XAudio2.h>
#include <codec.h>
#include <vorbisfile.h>

Voice::Voice(IXAudio2* IXA2, int buffercount, int buffersize)
{
	pXAudio2 = IXA2;
	buffers = new char*[buffercount];
	for (int i = 0; i < buffercount; i++)
		buffers[i] = new char[buffersize];
	count = buffercount;
	size = buffersize;
	bRunning = false;
	bPaused = false;
	bAlmostDone = false;
	bDone = false;
	bInUse = true;
	bLoop = false;
	sampleRate = 0;
	currentBuffer = 0;
	vf = new OggVorbis_File;
}

Voice::~Voice()
{
	if (pSourceVoice)
	{
		pSourceVoice->Stop();
		pSourceVoice->DestroyVoice();
	}

	for (int i = 0; i < count; i++)
		delete[] buffers[i];
	delete[] buffers;

	if (vf)
		ov_clear(vf);
	delete vf;
}

bool Voice::Load(const char* filename)
{
	FILE* file;
	if (fopen_s(&file, filename, "rb"))
		return false;
	
	if (ov_open_callbacks(file, vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
	{
		fclose(file);
		return false;
	}

	vorbis_info* vi = ov_info(vf, -1);

	WAVEFORMATEX wfm = { 0 };

	wfm.cbSize = sizeof(WAVEFORMATEX);
	wfm.nChannels = vi->channels;
	wfm.wBitsPerSample = 16; //OGG is always 16-bit
	wfm.nSamplesPerSec = vi->rate;
	wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nChannels * 2;
	wfm.nBlockAlign = 2 * wfm.nChannels;
	wfm.wFormatTag = 1;
	//ogg_int64_t a = ov_pcm_total(vf, -1);
	sampleRate = wfm.nSamplesPerSec;

	delete vi;

	DWORD pos = 0;
	int sec = 0;
	int ret = 1;

	memset(buffers[currentBuffer], 0, size);

	while (ret > 0 && pos < size)
	{
		ret = ov_read(vf, buffers[currentBuffer] + pos, size - pos, 0, 2, 1, &sec);
		pos += ret;
	}

	if (FAILED(pXAudio2->CreateSourceVoice(&pSourceVoice, &wfm)))
		return false;

	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = (BYTE*)buffers[currentBuffer];
	buffer.AudioBytes = size;

	if (FAILED(pSourceVoice->SubmitSourceBuffer(&buffer)))
		return false;

	currentBuffer++;

	return true;
}

bool Voice::Play(bool loop)
{
	if (!pSourceVoice)
		return false;

	if (FAILED(pSourceVoice->Start()))
		return false;

	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);
	bRunning = state.BuffersQueued > 0;

	bLoop = loop;
	bDone = false;
	bAlmostDone = false;
	bPaused = false;

	return bRunning;
}

void Voice::Update()
{
	if (!pSourceVoice)
		return;

	if (!bRunning)
		return;

	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);

	if (state.BuffersQueued < count - 1)
	{
		if (bDone && !bLoop)
			Stop();

		if (bAlmostDone && !bLoop)
			bDone = true;

		memset(buffers[currentBuffer], 0, size);

		DWORD pos = 0;
		int sec = 0;
		int ret = 1;

		while (ret > 0 && pos < size)
		{
			ret = ov_read(vf, buffers[currentBuffer] + pos, size - pos, 0, 2, 1, &sec);
			pos += ret;
		}

		if (!ret && bLoop) //check if ret == 0 (end of vorbis file stream)
		{
			ret = 1;
			ov_pcm_seek(vf, 0);
			while (ret > 0 && pos < size)
			{
				ret = ov_read(vf, buffers[currentBuffer] + pos, size - pos, 0, 2, 1, &sec);
				pos += ret;
			}
		}
		else if (!ret && !bLoop)
			bAlmostDone = true;

		XAUDIO2_BUFFER buffer = { 0 };
		buffer.pAudioData = (BYTE*)buffers[currentBuffer];
		buffer.AudioBytes = size;
		if (bAlmostDone && !bLoop)
			buffer.Flags = XAUDIO2_END_OF_STREAM;

		if (FAILED(pSourceVoice->SubmitSourceBuffer(&buffer)))
			return;

		currentBuffer++;
		currentBuffer %= count;
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

	bPaused = false;
	bRunning = false;
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
	pSourceVoice->GetVolume(&volume);
	return volume;
}

int Voice::GetSamplesPlayed()
{
	XAUDIO2_VOICE_STATE state;
	pSourceVoice->GetState(&state);
	return state.SamplesPlayed;
}