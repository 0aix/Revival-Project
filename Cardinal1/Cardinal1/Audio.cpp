#include "Audio.h"

namespace Audio
{
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	RAWSFXList* pRAWSFXBase;
	VoiceList* pVoiceBase;
	std::mutex mtx1; //For RAWSFX
	std::mutex mtx2; //For Voice

	bool Initialize()
	{
		if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
			return false;

		if (FAILED(XAudio2Create(&pXAudio2)))
			return false;

		if (FAILED(pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
			return false;

		pRAWSFXBase = new RAWSFXList;
		pVoiceBase = new VoiceList;

		return true;
	}

	void Uninitialize()
	{
		RAWSFXList* curr1 = pRAWSFXBase->pNext;
		RAWSFXList* temp1;
		mtx1.lock();
		while (curr1 != NULL)
		{
			temp1 = curr1;
			curr1 = curr1->pNext;
			temp1->pRAWSFX->Kill();
			delete temp1->pRAWSFX;
			delete temp1;
		}
		delete pRAWSFXBase;
		mtx1.unlock();

		VoiceList* curr2 = pVoiceBase->pNext;
		VoiceList* temp2;
		mtx2.lock();
		while (curr2 != NULL)
		{
			temp2 = curr2;
			curr2 = curr2->pNext;
			delete temp2->pVoice;
			delete temp2;
		}
		delete pVoiceBase;
		mtx2.unlock();

		if (pMasteringVoice)
			pMasteringVoice->DestroyVoice();

		if (pXAudio2)
			pXAudio2->Release();

		CoUninitialize();
	}

	void Update()
	{
		//pRAWSFX is a start node
		RAWSFXList* prev1 = pRAWSFXBase;
		RAWSFXList* curr1 = pRAWSFXBase->pNext;

		mtx1.lock();

		while (curr1 != NULL) //iterate through list; remove and free up voices that have exited
		{
			RAWSFX* pRAWSFX = curr1->pRAWSFX;
			if (pRAWSFX->bDone)
			{
				prev1->pNext = curr1->pNext;
				delete pRAWSFX;
				delete curr1;
				curr1 = prev1->pNext;
			}
		}

		mtx1.unlock();

		//pVoiceBase is a start node
		VoiceList* prev2 = pVoiceBase;
		VoiceList* curr2 = pVoiceBase->pNext;

		mtx2.lock();

		while (curr2 != NULL) //iterate through list; remove and free up voices that have exited
		{
			Voice* pVoice = curr2->pVoice;
			if (pVoice->bRunning)
				pVoice->Stream();
			if (!pVoice->bExit)
			{
				prev2 = curr2;
				curr2 = curr2->pNext;
			}
			else
			{
				prev2->pNext = curr2->pNext;
				delete pVoice;
				delete curr2;
				curr2 = prev2->pNext;
			}
		}
		
		mtx2.unlock();
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

	RAWSFX* PlayRAWSFX(RAWSOUND* raw)
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

	Sound* CreateSound(BUFFER* buffer)
	{
		Sound* sound = new Sound;
		sound->vf = new OggVorbis_File;
		if (ov_open_callbacks(buffer, sound->vf, NULL, 0, BUFFER::OV_CALLBACKS_BUFFER) < 0)
		{
			delete sound;
			return NULL;
		}
		vorbis_info* vi = ov_info(sound->vf, -1);
		
		sound->wfm.cbSize = sizeof(WAVEFORMATEX);
		sound->wfm.nChannels = vi->channels;
		sound->wfm.wBitsPerSample = 16; //OGG is always 16-bit
		sound->wfm.nSamplesPerSec = vi->rate;
		sound->wfm.nAvgBytesPerSec = vi->rate * vi->channels * 2;
		sound->wfm.nBlockAlign = 2 * vi->channels;
		sound->wfm.wFormatTag = 1;

		return sound;
	}

	Voice* CreateVoice()
	{
		mtx2.lock();

		Voice* voice = new Voice(pXAudio2);
		VoiceList* node = new VoiceList;
		node->pVoice = voice;
		node->pNext = pVoiceBase->pNext;
		pVoiceBase->pNext = node;

		mtx2.unlock();
		return voice;
	}
}