#include "stdafx.h"
#include "Game.h"
#include "Graphics.h"
#include "Scene.h"

namespace Game
{
	HANDLE audio;
	HANDLE update;
	HANDLE render;
	bool bExit = false;

	bool Initialize(HWND hwnd)
	{
		if (!Graphics::Initialize(hwnd))
			return false;

		/////////////////////////////////

		Cardinal::NextScene(new TestScene());

		/////////////////////////////////

		update = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UpdateThread, NULL, 0, NULL);
		//render = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RenderThread, NULL, 0, NULL);

		if (!update)
			return false;

		return true;
	}

	void Uninitialize()
	{
		bExit = true;
		WaitForMultipleObjects(3, new HANDLE[3] { audio, update, render }, true, INFINITE);
		CloseHandle(audio);
		CloseHandle(update);
		CloseHandle(render);

		Cardinal::ReleaseScene();
		Graphics::Uninitialize();
	}

	void UpdateThread()
	{
		for (; !bExit; Sleep(5))
			Cardinal::UpdateScene();
	}

	void RenderThread()
	{
		for (; !bExit; Sleep(1))
			Cardinal::RenderScene();
	}
}