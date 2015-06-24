#ifndef SCENE_H
#define SCENE_H

#include <fstream>
#include "Main.h"
#include "Misc.h"
#include "Graphics.h"

class Subscene;
class ViewManager;

class Scene
{
public:
	Scene()
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
	Subscene** pScenes;
};

class MainScene : public Scene
{
public:
	MainScene();
	void Release();
	void Update();
	void HandleInput();
	void Render();

private:
	ViewManager* VM;

};

class LoadScene : public Scene
{
public:
	LoadScene();
	void Release();
	void Update();
	void HandleInput();
	void Render();

private:
	int tick = 0;
};

class EndScene : public Scene
{
public:
	EndScene();
	void Release();
	void Update();
	void HandleInput();
	void Render();

};

#endif