#include "Audio.h"
#include "Voice.h"
#include <XAudio2.h>
#include <codec.h>
#include <vorbisfile.h>

namespace Audio
{
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	VoiceList* pVoiceBase;

	bool Initialize()
	{
		if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
			return false;

		if (FAILED(XAudio2Create(&pXAudio2)))
			return false;

		if (FAILED(pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
			return false;

		pVoiceBase = new VoiceList;
		pVoiceBase->pVoice = NULL;
		pVoiceBase->pNext = NULL;

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
		{
			pXAudio2->Release();
			pXAudio2 = NULL;
		}

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
}