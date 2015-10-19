#pragma once

#include "Cardinal.h"
#include "Graphics.h"
#include "Misc.h"
#include <Box2D.h>

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
};

class TestScene : public IScene
{
public:
	TestScene();
	void Release();
	void Update();
	void HandleInput();
	void Render();

private:
	b2Vec2 gravity = b2Vec2(0.0f, 32.0f);
	b2World world = b2World(gravity);
	b2Body* groundBody;
	b2Body* body;

	Clock clock;
	double dTime;
	float angle = 0.0f;
	bool toggle = false;
	double rcount = 0.0;
	double ucount = 0.0;
	double tcount = 0.0;
	double time = 0.0;

	IDirect3DTexture9* square;
	D3DXVECTOR2 scaling;
	D3DXMATRIX scalingmat;
};