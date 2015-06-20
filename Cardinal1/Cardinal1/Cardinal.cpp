#include "Cardinal.h"
#include "Jotunheimr.h"
#include "Game.h"
#include "Misc.h"
#include "Scene.h"

namespace Cardinal
{
	void** pStates;
	Input* pInput = NULL;
	Scene* pScene;
	RAWINPUT* pBuffer;
	DWORD dwSize;

	HWND hWnd;
	mutex mtx;
	const int STATE_COUNT = 1000;
	const int BUFFER_SIZE = 32;
	int start = 0;
	int end = 0;
	
	bool Initialize(HWND hwnd)
	{
		pStates = new void*[STATE_COUNT];
		pInput = new Input { 0 };
		pBuffer = new RAWINPUT[BUFFER_SIZE];
		
		hWnd = hwnd;
		
		if (!Jotunheimr::Initialize())
			return false;
		
		if (!Game::Initialize(hwnd))
			return false;

		return true;
	}

	void Uninitialize()
	{
		Game::Uninitialize();
		Jotunheimr::Uninitialize();

		delete pBuffer;
		delete pScene;
		delete pInput;
		delete pStates;
	}

	void End()
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	}

	void SetState(int index, void* state)
	{
		if (0 <= index && index < STATE_COUNT)
			pStates[index] = state;
	}

	void* GetState(int index)
	{
		return pStates[index];
	}

	bool RegisterInputDevices()
	{
		RAWINPUTDEVICE RID[2];
		RID[0].usUsagePage = 1;
		RID[0].usUsage = 2;
		RID[0].dwFlags = RIDEV_NOLEGACY;
		RID[0].dwFlags = 0;
		RID[0].hwndTarget = hWnd;
		RID[1].usUsagePage = 1;
		RID[1].usUsage = 6;
		RID[1].dwFlags = RIDEV_NOLEGACY;
		RID[1].dwFlags = 0;
		RID[1].hwndTarget = hWnd;
		return RegisterRawInputDevices(RID, 2, sizeof(RAWINPUTDEVICE));
	}

	void BufferInput(HRAWINPUT handle)
	{
		dwSize = sizeof(RAWINPUT);
		GetRawInputData(handle, RID_INPUT, pBuffer + end, (PUINT)&dwSize, sizeof(RAWINPUTHEADER));
		end++;
		if (end == BUFFER_SIZE)
			end = 0;
		if (end == start) //stupid fix; rewrites last input if overloading
		{
			end--;
			if (end == -1)
				end = BUFFER_SIZE - 1;
		}
	}

	void HandleBufferedInput()
	{
		if (pInput->CLR)
		{
			pScene->HandleInput();
			ZeroMemory(pInput, sizeof(Input));
			return;
		}

		RAWINPUT raw;

		int limit = end;
		while (start != limit)
		{
			raw = pBuffer[start];
			if (raw.header.dwType == RIM_TYPEMOUSE)
			{
				RAWMOUSE mouse = raw.data.mouse;
				pInput->type = 0;
				if (mouse.usFlags & 0x01) // absolute
				{
					pInput->pos.x = mouse.lLastX;
					pInput->pos.y = mouse.lLastY;
					pInput->relative = false;
				}
				else // relative
				{
					pInput->pos.x += mouse.lLastX;
					pInput->pos.y += mouse.lLastY;
					pInput->relative = true;
				}
				pInput->DX = mouse.lLastX;
				pInput->DY = mouse.lLastY;
				pInput->L = mouse.usButtonFlags;
				pInput->R = mouse.usButtonFlags >> 0x02;
				pInput->M = mouse.usButtonFlags >> 0x04;
				if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
				{
					pInput->scroll = true;
					pInput->D = mouse.usButtonData;
				}
				else
					pInput->scroll = false;
				pInput->rawbuttons = mouse.ulRawButtons;
			}
			else if (raw.header.dwType == RIM_TYPEKEYBOARD)
			{
				RAWKEYBOARD keyboard = raw.data.keyboard;
				byte vk = keyboard.VKey;
				if (vk < 146)
				{
					bool flag = ~keyboard.Flags & 0x01;
					pInput->keycode = vk;
					pInput->state = flag;
					pInput->VK[vk] = flag;
					if (vk == VK_CONTROL)
						pInput->CTRL = flag;
					else if (vk == VK_SHIFT)
						pInput->SHIFT = flag;
					else if (vk == VK_MENU)
						pInput->ALT = flag;
				}
			}
			start++;
			if (start == BUFFER_SIZE)
				start = 0;
		}
		pScene->HandleInput();
	}

	int LoadScene(Scene* ps)
	{
		return 0;
	}

	void NextScene(Scene* ps)
	{
		pScene = ps;
	}

	void UpdateScene()
	{
		mtx.lock();
		HandleBufferedInput();
		pScene->Update();
		mtx.unlock();
	}

	void RenderScene()
	{
		mtx.lock();
		pScene->Render();
		mtx.unlock();
	}
}