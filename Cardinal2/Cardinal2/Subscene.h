#pragma once

#include "Scene.h"
#include "World.h"

class ISubscene : public IScene
{
public:
	ISubscene() : IScene() {}

protected:
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	bool bActive = true;
	bool bVisible = true;
};

class WorldScene : public ISubscene
{
public:
	WorldScene(int width, int length, int cap);
	void Release();
	void Update();
	void HandleInput();
	void Render();

private:
	GameClock clock;
	ICharacter* player;
	World* world;
	D3DXVECTOR3 camera;
	double dTime;

	D3DXMATRIX scalingmat;
	D3DXVECTOR2 scaling;
	IDirect3DTexture9* circle = NULL;
	IDirect3DTexture9* arrow = NULL;
	IDirect3DTexture9* tile = NULL;

	D3DXVECTOR3 camera_offset;
	double xbound;
	double ybound;
};