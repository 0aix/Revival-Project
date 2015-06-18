#include "Jotunheimr.h"
#include "Graphics.h"
#include <fstream>

namespace Jotunheimr
{
	const BYTE VERSION = 0;
	const WORD TABLE_COUNT = 5;
	const char HEADER[] = { 'T', 'R', 'P', 'H' };
	DWORD dwAllocGran = 0;

	//
	bool bExit = false;
	const DWORD VIEW_SIZE_LIMIT = 512 * 1024; //512KB per table
	const int WORKER_COUNT = 4;
	const int PACKER_COUNT = 4;

	Manager manager;
	Worker worker[WORKER_COUNT];
	Checker checker;
	Packer packer[PACKER_COUNT];


	//ARRAY IMPLEMENTATION
	//CHANGE TO BINARY TREE / HASH TABLE LATER

	TOFILE TOFile[TABLE_COUNT];
	DWORD* ListSize[TABLE_COUNT];
	DWORD* ListOffset[TABLE_COUNT];
	DWORD ItemCount[TABLE_COUNT];
	Res* ResList[TABLE_COUNT];

	bool Initialize()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		dwAllocGran = info.dwAllocationGranularity;

		if (!LoadListTO())
			return false;

		if (!LoadTO(TO::SPRITE, "Sprite.to") ||
			!LoadTO(TO::SOUND, "Sound.to") ||
			!LoadTO(TO::STRING, "String.to") ||
			!LoadTO(TO::ANIM, "Anim.to") ||
			!LoadTO(TO::PACK, "Pack.to"))
			return false;

		for (int i = 0; i < WORKER_COUNT; i++)
			worker[i].hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkerThread, (worker + i), 0, NULL);
		manager.hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ManagerThread, NULL, 0, NULL);

		for (int i = 0; i < PACKER_COUNT; i++)
			packer[i].hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PackerThread, (packer + i), 0, NULL);
		checker.hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckerThread, NULL, 0, NULL);

		return true;
	}

	void Uninitialize()
	{
		bExit = true;
		for (int i = 0; i < WORKER_COUNT; i++)
		{
			SetEvent(worker[i].hEvent);
			WaitForSingleObject(worker[i].hThread, INFINITE);
		}
		WaitForSingleObject(manager.hThread, INFINITE);

		for (int i = 0; i < PACKER_COUNT; i++)
		{
			SetEvent(packer[i].hEvent);
			WaitForSingleObject(packer[i].hThread, INFINITE);
		}
		WaitForSingleObject(checker.hThread, INFINITE);

		for (int i = 0; i < TABLE_COUNT; i++)
		{
			CloseHandle(TOFile[i].hMapFile);
			CloseHandle(TOFile[i].hFile);
		}

		for (int i = 0; i < TABLE_COUNT; i++)
		{
			for (DWORD n = 0; n < ItemCount[i]; n++)
				if (ResList[i][n].state == 2)
					UnloadResource(i, n);

			delete[] ListSize[i];
			delete[] ListOffset[i];
			delete[] ResList[i];
		}
	}

	bool LoadListTO()
	{
		HANDLE hFile = CreateFile("List.to", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;
		HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (!hMapFile)
			return false;
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		BYTE* pBase = (BYTE*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, dwFileSize); //List.to is small enough to load all at once

		//Check header and version
		if (!pBase)
			return false;
		if (memcmp(HEADER, pBase, 4) != 0)
			return false;
		if (*(pBase + 4) != VERSION)
			return false;

		//Check table type and count
		if (*(pBase + 5) != TO::LIST)
			return false;
		if (*(WORD*)(pBase + 6) != TABLE_COUNT)
			return false;

		//Enumerate file sizes and offsets
		DWORD nOffset = 8;
		for (int i = 0; i < TABLE_COUNT; i++)
		{
			//Check individual table type
			if (*(pBase + nOffset) != i)
				return false;

			WORD count = *(WORD*)(pBase + nOffset + 1);

			DWORD* sizes = new DWORD[count];
			DWORD* offsets = new DWORD[count];
			ItemCount[i] = count;
			BYTE num = *(pBase + nOffset + 3);
			nOffset += 4;
			DWORD fOffset = 8; //Offset for file data
			DWORD mask = (DWORD)0xFFFFFFFF >> (4 - num) * 8;
			for (int n = 0; n < count; n++)
			{
				DWORD size = *(DWORD*)(pBase + nOffset) & mask;
				sizes[n] = size;
				offsets[n] = fOffset;
				fOffset += size;
				nOffset += num;
			}
			ListSize[i] = sizes;
			ListOffset[i] = offsets;

			if (GetLastError() != ERROR_SUCCESS)
				return false;
		}

		UnmapViewOfFile(pBase);
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return true;
	}

	bool LoadTO(int type, char* filename)
	{
		HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		//Check header and version
		char buffer[4];
		if (!ReadFile(hFile, buffer, 4, NULL, NULL))
			return false;
		if (memcmp(HEADER, buffer, 4) != 0)
			return false;
		if (!ReadFile(hFile, buffer, 1, NULL, NULL))
			return false;
		if (*(BYTE*)buffer != VERSION)
			return false;

		//Check table type
		if (!ReadFile(hFile, buffer, 3, NULL, NULL))
			return false;
		if (*(BYTE*)buffer != type)
			return false;

		//Get the entry count
		WORD count = *(WORD*)(buffer + 1);
		if (count != ItemCount[type])
			return false;
		ResList[type] = new Res[count];

		//Create file mapping object
		HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (!hMapFile)
			return false;

		//Construct TOFILE
		TOFile[type].hFile = hFile;
		TOFile[type].hMapFile = hMapFile;
		TOFile[type].dwSize = GetFileSize(hFile, NULL);

		return true;
	}

	bool LoadResource(int type, int ID, void** pObj)
	{
		if (pObj)
			*pObj = ResList[type][ID].loc;
		if (ResList[type][ID].state == 2)
			return true;
		if (ResList[type][ID].state == 0) //Not loaded
		{
			Item* item = new Item;
			item->type = type;
			item->ID = ID;
			manager.mtx.lock();
			if (!manager.hThread)
				manager.hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ManagerThread, NULL, 0, NULL);
			Item* end = manager.pEnd;
			end->pNext->pNext = item;
			item->pNext = end;
			end->pNext = item;
			ResList[type][ID].state = 1; //Loading
			manager.mtx.unlock();
		}
		return false;
	}

	void UnloadResource(int type, int ID)
	{
		Res* obj = &ResList[type][ID];
		if (obj->loc && obj->state == 2) //Loaded
		{
			if (type == TO::SPRITE)
				((IDirect3DTexture9*)obj->loc)->Release();
			else if (type == TO::SOUND)
			{
				//HAWKWARD
			}
			else if (type == TO::STRING)
				delete (STRING*)obj->loc;
			else if (type == TO::ANIM)
			{
				Anim* anim = (Anim*)obj->loc;
				Frame* frame = anim->frame;
				BYTE count = anim->count;
				for (BYTE i = 0; i < count; i++)
					UnloadResource(TO::SPRITE, frame[i].ID);
				delete anim;
			}
			else if (type == TO::PACK)
			{
				Pack* pack = (Pack*)obj->loc;
				BYTE* type = pack->type;
				WORD* ID = pack->ID;
				BYTE count = pack->count;
				for (BYTE i = 0; i < count; i++)
					UnloadResource(type[i], ID[i]);
				delete pack;
			}
			obj->loc = NULL;
			obj->state = 0; //Not loaded
		}
	}

	void LoadPackage(Package* package)
	{
		checker.mtx.lock();
		if (!checker.hThread)
			checker.hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckerThread, NULL, 0, NULL);
		Package* end = checker.pEnd;
		end->pNext->pNext = package;
		package->pNext = end;
		end->pNext = package;
		checker.mtx.unlock();
	}

	void ManagerThread()
	{
		Item* base = manager.pBase;
		Item* end = manager.pEnd;
		try
		{
			for (; !bExit; Sleep(500))
			{
				manager.mtx.lock();
				Item* curr = base->pNext;
				for (int i = 0; curr != end; i = (i + 1) % WORKER_COUNT)
				{
					if (!worker[i].hThread)
					{
						ResetEvent(worker[i].hEvent);
						worker[i].bDone = worker[i].pItem == NULL;
						worker[i].hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkerThread, (worker + i), 0, NULL);
					}
					if (worker[i].bDone && curr != end)
					{
						BYTE type = curr->type;
						WORD ID = curr->ID;
						DWORD itemsize = ListSize[type][ID];
						DWORD itemoffset = ListOffset[type][ID];
						FILEVIEW* fView = manager.fView[type];
						FILEVIEW* prev = NULL;
						bool found = false;
						while (fView != NULL)
						{
							if (fView->dwOffset <= itemoffset && itemoffset + itemsize <= fView->dwSize)
							{
								worker[i].pItem = curr;
								worker[i].pView = fView;
								InterlockedIncrement(&fView->dwRef);
								found = true;
								break;
							}
							if (manager.fViewSize[i] > VIEW_SIZE_LIMIT && fView->dwRef == 0)
							{
								if (prev)
									prev->pNext = fView->pNext;
								else
									manager.fView[type] = fView->pNext;
								UnmapViewOfFile(fView->pBase);
								delete fView;
								fView = prev->pNext;
								continue;
							}
							prev = fView;
							fView = fView->pNext;
						}
						if (!found)
						{
							FILEVIEW* pView = new FILEVIEW;
							DWORD page_offset = (itemoffset / dwAllocGran) * dwAllocGran;
							DWORD page_end = ((itemoffset + itemsize) / dwAllocGran + 1) * dwAllocGran;
							if (page_end > TOFile[type].dwSize)
								page_end = TOFile[type].dwSize;
							pView->pBase = (BYTE*)MapViewOfFile(TOFile[type].hMapFile, FILE_MAP_READ, 0, page_offset, page_end - page_offset);
							pView->dwSize = page_end;
							pView->dwOffset = page_offset;
							pView->dwRef = 1;
							pView->pNext = manager.fView[type];
							manager.fView[type] = pView;

							worker[i].pItem = curr;
							worker[i].pView = pView;
						}
						worker[i].bDone = false;
						SetEvent(worker[i].hEvent);

						curr = curr->pNext;
						base->pNext = curr;
						if (curr == end)
							end->pNext = base;
					}
				}
				manager.mtx.unlock();
			}
		}
		catch (...)
		{
			CloseHandle(manager.hThread);
			manager.hThread = NULL;
			manager.mtx.unlock();
		}
	}

	void WorkerThread(void* param)
	{
		Worker* worker = (Worker*)param;
		try
		{
			while (!bExit)
			{
				if (worker->bDone)
				{
					if (WaitForSingleObject(worker->hEvent, INFINITE) != WAIT_OBJECT_0)
						throw 0;
					ResetEvent(worker->hEvent);
					continue;
				}
				
				Item* pItem = worker->pItem;
				FILEVIEW* pView = worker->pView;
				WORD type = pItem->type;
				WORD ID = pItem->ID;
				DWORD offset = ListOffset[type][ID];
				DWORD size = ListSize[type][ID];
				BYTE* base = pView->pBase + (offset - pView->dwOffset);
				if (type == TO::SPRITE)
				{
					IDirect3DTexture9* texture;
					HRESULT r = D3DXCreateTextureFromFileInMemory(Graphics::d3ddev, base, size, &texture);
					ResList[type][ID].loc = texture;
					ResList[type][ID].state = 2; //Loaded
				}
				else if (type == TO::SOUND)
				{
					//HAAAAWKWARD
				}
				else if (type == TO::STRING)
				{
					STRING* str = new STRING(size);
					memcpy(str->cstr, base, size);
					ResList[type][ID].loc = str;
					ResList[type][ID].state = 2; //Loaded
				}
				else if (type == TO::ANIM)
				{
					BYTE count = *base;
					Anim* anim = new Anim(count, *(base + 1)); //The dereference is for loops
					DWORD index = 2; //Offset for frames
					for (BYTE i = 0; i < count; i++)
					{
						WORD v = *(WORD*)(base + index);
						LoadResource(TO::SPRITE, v, NULL);
						anim->obj[i] = &ResList[TO::SPRITE][v];
						anim->frame[i].ID = v;
						anim->frame[i].x = *(WORD*)(base + index + 2);
						anim->frame[i].y = *(WORD*)(base + index + 4);
						anim->frame[i].delay = *(WORD*)(base + index + 6);
						index += 8;
					}
					Package* package = new Package;
					package->type = type;
					package->ID = ID;
					package->package = anim;
					LoadPackage(package);
				}
				else if (type == TO::PACK)
				{
					BYTE count = *base;
					Pack* pack = new Pack(count);
					DWORD index = 1; //Offset for items
					for (BYTE i = 0; i < count; i++)
					{
						BYTE b = *(base + index);
						WORD v = *(WORD*)(base + index + 1);
						LoadResource(b, v, NULL);
						pack->obj[i] = &ResList[b][v];
						pack->type[i] = b;
						pack->ID[i] = v;
						index += 3;
					}
					Package* package = new Package;
					package->type = type;
					package->ID = ID;
					package->package = pack;
					LoadPackage(package);
				}

				delete pItem;
				InterlockedDecrement(&pView->dwRef);
				worker->pItem = NULL;
				worker->pView = NULL;
				worker->bDone = true;
			}
		}
		catch (...) 
		{
			CloseHandle(worker->hThread);
			worker->hThread = NULL;
		}
	}

	void CheckerThread()
	{
		Package* base = checker.pBase;
		Package* end = checker.pEnd;
		try
		{
			for (; !bExit; Sleep(500))
			{
				checker.mtx.lock();
				Package* curr = base->pNext;
				Package* prev = base;
				while (curr != end)
				{
					bool queue = true;
					Res** obj = NULL;
					BYTE count = 0;
					if (curr->type == TO::ANIM)
					{
						Anim* anim = (Anim*)curr->package;
						obj = anim->obj;
						count = anim->count;
					}
					else if (curr->type == TO::PACK)
					{
						Pack* pack = (Pack*)curr->package;
						obj = pack->obj;
						count = pack->count;
					}
					for (BYTE i = 0; i < count; i++)
					{
						if (obj[i]->state != 2) //Loaded
						{
							queue = false;
							break;
						}
					}

					if (queue)
					{
						for (int i = 0;; i = (i + 1) % PACKER_COUNT)
						{
							if (!packer[i].hThread)
							{
								ResetEvent(packer[i].hEvent);
								packer[i].bDone = packer[i].package == NULL;
								packer[i].hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PackerThread, (packer + i), 0, NULL);
							}
							if (packer[i].bDone && curr != end)
							{
								packer[i].package = curr;
								packer[i].bDone = false;
								SetEvent(packer[i].hEvent);
								prev->pNext = curr->pNext;
								curr = prev;
								break;
							}
						}
					}
					prev = curr;
					curr = curr->pNext;
				}
				checker.mtx.unlock();
			}
		}
		catch (...)
		{
			CloseHandle(checker.hThread);
			checker.hThread = NULL;
			checker.mtx.unlock();
		}
	}

	void PackerThread(void* param)
	{
		Packer* packer = (Packer*)param;
		try
		{
			while (!bExit)
			{
				if (packer->bDone)
				{
					if (WaitForSingleObject(packer->hEvent, INFINITE) != WAIT_OBJECT_0)
						throw 0;
					ResetEvent(packer->hEvent);
					continue;
				}

				Package* package = packer->package;
				if (package->type == TO::ANIM)
				{
					Anim* anim = (Anim*)package->package;
					Res** obj = anim->obj;
					Frame* frame = anim->frame;
					BYTE count = anim->count;
					bool loop = anim->loops > 0; //For now, either loops or doesn't
					SpriteAnim* spriteanim = new SpriteAnim(count, loop);
					Sprite* sprites = spriteanim->sprites;
					for (BYTE i = 0; i < count; i++)
					{
						sprites[i].texture = (IDirect3DTexture9*)obj[i]->loc;
						sprites[i].center = D3DXVECTOR3(frame[i].x, frame[i].y, 0.0f);
						sprites[i].delay = (float)frame[i].delay / 1000.0f;
					}
					anim->spriteanim = spriteanim;

					ResList[package->type][package->ID].loc = anim;
					ResList[package->type][package->ID].state = 2; //Loaded
				}
				else if (package->type == TO::PACK)
				{
					Pack* pack = (Pack*)package->package;
					Res** obj = pack->obj;
					BYTE count = pack->count;
					void** fullpack = new void*[count];
					for (BYTE i = 0; i < count; i++)
						fullpack[i] = obj[i]->loc;
					pack->pack = fullpack;

					ResList[package->type][package->ID].loc = pack;
					ResList[package->type][package->ID].state = 2; //Loaded
				}

				delete package;
				packer->package = NULL;
				packer->bDone = true;
			}
		}
		catch (...)
		{
			CloseHandle(packer->hThread);
			packer->hThread = NULL;
		}
	}
}