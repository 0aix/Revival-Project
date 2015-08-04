#pragma once

#include "Windows.h"

namespace Game
{
	bool Initialize(HWND hwnd);
	void Uninitialize();

	void AudioThread();
	void UpdateThread();
	void RenderThread();
};