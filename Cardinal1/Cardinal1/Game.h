#ifndef GAME_H
#define GAME_H

#include "Main.h"

namespace Game
{
	 bool Initialize(HWND hwnd);
	 void Uninitialize();

	 void AudioThread();
	 void UpdateThread();
	 void RenderThread();
};

#endif