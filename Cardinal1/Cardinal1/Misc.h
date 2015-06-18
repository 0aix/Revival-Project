#ifndef MISC_H
#define MISC_H

#include "Windows.h"

struct Input
{
	byte type; //0 == mouse, 1 == keyboard

	POINT pos;
	bool relative;
	byte L; //0 nothing, 1 down, 2 up
	byte M;
	byte R;
	bool scroll;
	int D;
	DWORD rawbuttons;
	int DX;
	int DY;

	byte keycode;
	bool state; //true == down, false == up
	bool VK[146];
	bool CTRL;
	bool SHIFT;
	bool ALT;

	bool CLR;
};

struct GameClock
{
	float frequency;
	__int64 ticks;

	void Start()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		frequency = (float)li.QuadPart;

		QueryPerformanceCounter(&li);
		ticks = li.QuadPart;
	}

	float Time()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);

		float elapsed = (float)(li.QuadPart - ticks) / frequency;
		ticks = li.QuadPart;

		return elapsed;
	}
};

#endif