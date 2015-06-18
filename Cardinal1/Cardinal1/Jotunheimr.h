#ifndef JOTUNHEIMR_H
#define JOTUNHEIMR_H

#include "Main.h"
#include "Sprite.h"
#include "TO.h"

namespace TO
{
	const BYTE SPRITE = 0x00;
	const BYTE SOUND = 0x01;
	const BYTE STRING = 0x02;
	const BYTE ANIM = 0x03;
	const BYTE PACK = 0x04;
	const BYTE LIST = 0xFF;
}

struct TOFILE
{
	HANDLE hFile;
	HANDLE hMapFile;
	DWORD dwSize;
};

struct FILEVIEW
{
	BYTE* pBase = NULL;
	DWORD dwSize = 0; //This is the size of the file view, not the range!
	DWORD dwOffset = 0;
	DWORD dwRef = 0;
	FILEVIEW* pNext = NULL;
};

struct Res
{
	void* loc = NULL;
	BYTE state = 0; //0 - nothing, 1 - loading, 2 - loaded
};

struct Frame
{
	WORD ID;
	WORD x;
	WORD y;
	WORD delay;
};

struct Anim
{
	Res** obj;
	Frame* frame;
	BYTE count;
	BYTE loops;
	SpriteAnim* spriteanim;

	Anim(BYTE size, BYTE loopcount)
	{
		obj = new Res*[size];
		frame = new Frame[size];
		count = size;
		loops = loopcount;
	}

	~Anim() 
	{
		delete[] obj;
		delete[] frame;
		delete spriteanim;
	}
};

struct Pack
{
	Res** obj;
	BYTE* type;
	WORD* ID;
	BYTE count;
	void** pack;

	Pack(BYTE size)
	{
		obj = new Res*[size];
		type = new BYTE[size];
		ID = new WORD[size];
		count = size;
	}

	~Pack() 
	{
		delete[] obj;
		delete[] type;
		delete[] ID;
		delete[] pack;
	}
};

struct STRING
{
	char* cstr;
	WORD length;

	STRING(WORD size)
	{
		cstr = new char[size];
		length = size;
	}

	~STRING() { delete[] cstr; }
};

struct Item
{
	BYTE type;
	WORD ID;
	Item* pNext = NULL;
};

struct Manager
{
	HANDLE hThread;
	Item* pBase;
	Item* pEnd;
	FILEVIEW* fView[5];
	DWORD fViewSize[5];
	mutex mtx;

	Manager()
	{
		pBase = new Item;
		pEnd = new Item;
		pBase->pNext = pEnd;
		pEnd->pNext = pBase;
	}

	~Manager()
	{
		Item* curr = pBase->pNext;
		Item* temp;
		while (curr != pEnd)
		{
			temp = curr->pNext;
			delete curr;
			curr = temp;
		}
		delete pBase;
		delete pEnd;
		CloseHandle(hThread);
	}
};

struct Worker
{
	HANDLE hThread;
	HANDLE hEvent;
	Item* pItem = NULL;
	FILEVIEW* pView = NULL;
	bool bDone = true;

	Worker(){ hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); }

	~Worker()
	{
		CloseHandle(hEvent);
		CloseHandle(hThread);
	}
};

struct Package
{
	BYTE type;
	WORD ID;
	void* package;
	Package* pNext = NULL;
};

struct Checker
{
	HANDLE hThread;
	Package* pBase;
	Package* pEnd;
	mutex mtx;

	Checker()
	{
		pBase = new Package;
		pEnd = new Package;
		pBase->pNext = pEnd;
		pEnd->pNext = pBase;
	}

	~Checker()
	{
		Package* curr = pBase->pNext;
		Package* temp;
		while (curr != pEnd)
		{
			temp = curr->pNext;
			delete curr;
			curr = temp;
		}
		delete pBase;
		delete pEnd;
		CloseHandle(hThread);
	}
};

struct Packer
{
	HANDLE hThread;
	HANDLE hEvent;
	Package* package = NULL;
	bool bDone = true;

	Packer() { hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); }

	~Packer()
	{
		CloseHandle(hEvent);
		CloseHandle(hThread);
	}
};

namespace Jotunheimr
{
	bool Initialize();
	void Uninitialize();

	//ARRAY IMPLEMENTATION
	bool LoadListTO();
	bool LoadTO(int type, char* filename);
	bool LoadResource(int type, int ID, void** pObj);
	void UnloadResource(int type, int ID);
	void LoadPackage(Package* package);

	void ManagerThread();
	void WorkerThread(void* param);
	void CheckerThread();
	void PackerThread(void* param);
}

#endif