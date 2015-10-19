#pragma once

#include "Windows.h"

namespace Game
{
	bool Initialize(HWND hwnd);
	void Uninitialize();

	void UpdateThread();
	void RenderThread();
};