#include "Game.h"
#include "Audio.h"
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
		if (!Audio::Initialize())
			return false;

		if (!Graphics::Initialize(hwnd))
			return false;

		/////////////////////////////////

		Cardinal::NextScene(new MainScene());

		/////////////////////////////////

		audio = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AudioThread, NULL, 0, NULL);
		update = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UpdateThread, NULL, 0, NULL);
		render = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RenderThread, NULL, 0, NULL);

		if (!update || !render)
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

		Graphics::Uninitialize();
	}

	void AudioThread()
	{
		for (; !bExit; Sleep(100))
			Audio::Update();
	}

	void UpdateThread()
	{
		for (; !bExit; Sleep(5))
			Cardinal::UpdateScene();
	}

	void RenderThread()
	{
		for (; !bExit; Sleep(10))
			Cardinal::RenderScene();
	}
}