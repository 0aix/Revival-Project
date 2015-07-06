#pragma once

#include <Windows.h>

template <typename T>
struct LList
{
	T* pData = NULL;
	LList<T>* pNext = NULL;
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

	void Start()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		frequency = (double)li.QuadPart;

		QueryPerformanceCounter(&li);
		ticks = li.QuadPart;
	}

	double Time()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);

		double elapsed = (double)(li.QuadPart - ticks) / frequency;
		ticks = li.QuadPart;

		return elapsed;
	}
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
	//pBase should be deleted manually
};
