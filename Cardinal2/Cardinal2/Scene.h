#pragma once

#include "Cardinal.h"
#include "Audio.h"
#include "Graphics.h"
#include "Misc.h"

class ISubscene;
class IView;
class ViewManager;

class IScene
{
public:
	IScene()
	{
		d3d = Graphics::d3d;
		d3ddev = Graphics::d3ddev;
		sprite = Graphics::sprite;
		pInput = Cardinal::pInput;
	}
	virtual void Release() = 0; //must delete itself
	virtual void Update() = 0;
	virtual void HandleInput() = 0;
	virtual void Render() = 0;

protected:
	IDirect3D9* d3d;
	IDirect3DDevice9* d3ddev;
	ID3DXSprite* sprite;
	Input* pInput;
	ISubscene** pScenes;
};

class MainScene : public IScene
{
public:
	MainScene();
	void Release();
	void Update();
	void HandleInput();
	void Render();

private:
	ViewManager* VM = NULL;
	IDirect3DTexture9* circle = NULL;
};

class LoadScene : public IScene
{
public:
	LoadScene();
	void Release();
	void Update();
	void HandleInput();
	void Render();
};

class EndScene : public IScene
{
public:
	EndScene();
	void Release();
	void Update();
	void HandleInput();
	void Render();
};