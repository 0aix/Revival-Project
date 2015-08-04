#pragma once

struct Input;
class IScene;

namespace Cardinal
{
	extern Input* pInput;

	bool Initialize(HWND hwnd);
	void Uninitialize();
	void End();

	void SetState(int index, void* state);
	void* GetState(int index);

	bool RegisterInputDevices();
	void BufferInput(HRAWINPUT handle);
	void HandleBufferedInput();

	int LoadScene(IScene* ps);
	void NextScene(IScene* ps);
	void UpdateScene();
	void RenderScene();
	void ReleaseScene();
}