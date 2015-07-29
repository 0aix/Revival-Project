#pragma once

#include <Windows.h>

template <typename T>
struct LNode
{
	T* pData = NULL;
	LList<T>* pNext = NULL;
};

template <typename T>
struct LList
{
	LNode<T>* pBase = NULL;
	LNode<T>* pEnd = NULL;
	LNode<T>* pCurr = NULL;
	LNode<T>** ppPrev = &pBase;

	T* Begin();
	T* At();
	void Next();
	void Remove(bool del);
	void RemoveAll();

	void Push(T* pData);
	void Queue(T* pData);
	T* Pop();
};


struct Input
{
	BYTE type; //0 == mouse, 1 == keyboard

	POINT pos;
	bool relative;
	BYTE L; //0 nothing, 1 down, 2 up
	BYTE M;
	BYTE R;
	bool scroll;
	int D;
	DWORD rawbuttons;
	int DX;
	int DY;

	BYTE keycode;
	bool state; //true == down, false == up
	bool VK[146];
	bool CTRL;
	bool SHIFT;
	bool ALT;
	bool CLR;
};

struct GameClock
{
	double frequency;
	__int64 ticks;

	void Start();
	double Time();
};

struct BUFFER
{
	BYTE* pBase = NULL;
	DWORD dwSize = 0;
	DWORD dwPos = 0;

	BUFFER() {}
	BUFFER(DWORD length)
	{
		pBase = new BYTE[length];
		dwSize = length;
	}
	//delete pBase manually
};
