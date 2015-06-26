#include "Audio.h"
#include "Sound.h"
#include "Voice.h"
#include <XAudio2.h>
#include <vorbisfile.h>
#include <mutex>

namespace Audio
{
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	VoiceList* pVoiceBase;
	RAWSFXList* pRAWSFXBase;
	SoundList* pSoundBase;
	std::mutex mtx1; //For RAWSFX
	std::mutex mtx2; //For Sound

	bool Initialize()
	{
		if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
			return false;

		if (FAILED(XAudio2Create(&pXAudio2)))
			return false;

		if (FAILED(pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
			return false;

		pRAWSFXBase = new RAWSFXList;
		pSoundBase = new SoundList;

		return true;
	}

	void Uninitialize()
	{
		VoiceList* temp = NULL;;
		while (pVoiceBase != NULL)
		{
			temp = pVoiceBase;
			pVoiceBase = pVoiceBase->pNext;
			delete temp->pVoice;
			delete temp;
		}

		if (pMasteringVoice)
			pMasteringVoice->DestroyVoice();

		if (pXAudio2)
			pXAudio2->Release();

		CoUninitialize();
	}

	void Update() //Needs a mutex here and for createvoice
	{
		//pVoiceBase is only a start node
		VoiceList* prev = pVoiceBase;
		VoiceList* curr = pVoiceBase->pNext;
		while (curr != NULL) //iterate through list; remove and free up voices not in use
		{
			Voice* temp = curr->pVoice;
			if (temp->bInUse)
			{
				temp->Update();
				prev = curr;
				curr = curr->pNext;
			}
			else
			{
				prev->pNext = curr->pNext;
				delete curr;
				curr = prev->pNext;
			}
		}
	}

	Voice* CreateVoice(int buCount, int buSize)
	{
		Voice* temp = new Voice(pXAudio2, buCount, buSize);
		VoiceList* node = new VoiceList;
		node->pVoice = temp;
		node->pNext = pVoiceBase->pNext;
		pVoiceBase->pNext = node;
		return temp;
	}

	RAWSOUND* CreateRAWSOUND(BUFFER* buffer)
	{
		OggVorbis_File* vf = new OggVorbis_File;
		if (ov_open_callbacks(buffer, vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
			return NULL;
		vorbis_info* vi = ov_info(vf, -1);

		RAWSOUND* raw = new RAWSOUND;
		raw->wfm.cbSize = sizeof(WAVEFORMATEX);
		raw->wfm.nChannels = vi->channels;
		raw->wfm.wBitsPerSample = 16; //OGG is always 16-bit
		raw->wfm.nSamplesPerSec = vi->rate;
		raw->wfm.nAvgBytesPerSec = vi->rate * vi->channels * 2;
		raw->wfm.nBlockAlign = 2 * vi->channels;
		raw->wfm.wFormatTag = 1;

		delete vi;

		DWORD length = (DWORD)ov_pcm_total(vf, -1) * vi->channels * 2;
		DWORD pos = 0;
		int sec = 0;
		int ret = 1;

		BYTE* pBase = new BYTE[length];
		while (ret > 0 && pos < length)
		{
			ret = ov_read(vf, (char*)pBase + pos, length - pos, 0, 2, 1, &sec);
			pos += ret;
		}
		ov_clear(vf);
		delete vf;

		raw->buffer.pAudioData = pBase;
		raw->buffer.AudioBytes = length;
		raw->buffer.Flags = XAUDIO2_END_OF_STREAM;
		return raw;
	}

	RAWSFX* CreateRAWSFX(RAWSOUND* raw)
	{
		RAWSFX* sfx = new RAWSFX;
		if (FAILED(pXAudio2->CreateSourceVoice(&sfx->pSourceVoice, &raw->wfm, 0, XAUDIO2_DEFAULT_FREQ_RATIO, sfx, NULL, NULL)) ||
			FAILED(sfx->pSourceVoice->SubmitSourceBuffer(&raw->buffer)) ||
			FAILED(sfx->pSourceVoice->Start()))
		{
			delete sfx;
			return NULL;
		}

		mtx1.lock();
		RAWSFXList* node = new RAWSFXList;
		node->pRAWSFX = sfx;
		node->pNext = pRAWSFXBase->pNext;
		pRAWSFXBase->pNext = node;
		mtx1.unlock();
		return sfx;
	}

	Sound* CreateSound(BUFFER* buffer, bool loop)
	{
		OggVorbis_File* vf = new OggVorbis_File;
		if (ov_open_callbacks(buffer, vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
			return NULL;
		vorbis_info* vi = ov_info(vf, -1);

		Sound* sound = new Sound(loop);
		sound->vf = vf;
		sound->wfm.cbSize = sizeof(WAVEFORMATEX);
		sound->wfm.nChannels = vi->channels;
		sound->wfm.wBitsPerSample = 16; //OGG is always 16-bit
		sound->wfm.nSamplesPerSec = vi->rate;
		sound->wfm.nAvgBytesPerSec = vi->rate * vi->channels * 2;
		sound->wfm.nBlockAlign = 2 * vi->channels;
		sound->wfm.wFormatTag = 1;

		delete vi;
		return sound;
	}
}