#ifndef SUBSCENE_H
#define SUBSCENE_H

#include "Scene.h"
#include "Constants.h"

class Character;
class World;
struct BallList;
struct EffectList;

class Subscene : public Scene
{
public:
	Subscene()
	{
		d3d = Graphics::d3d;
		d3ddev = Graphics::d3ddev;
		sprite = Graphics::sprite;
		pInput = Cardinal::pInput;
	}

protected:
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	bool bActive = true;
	bool bVisible = true;
};

class BattleScene : public Subscene
{
public:
	BattleScene();
	void Release();
	void Update();
	void HandleInput();
	void Render();

private:
	World* world;
	Character* player;
	
};

class WorldScene : public Subscene
{
public:
	WorldScene(Character* c, World* w);
	void Release();
	void Update();
	void HandleInput();
	void Render();

private:
	GameClock Clock;
	World* world;
	Character* player;
	Character** players;
	BallList* pBallBase; //null start node
	EffectList* pEffectBase; //null start node
	D3DXVECTOR3 camera;
	IDirect3DTexture9** textures;
	IDirect3DTexture9* texture;
	float fTime = 0.0f;

};

class BattleUIScene : public Subscene
{
public:
	BattleUIScene();
	void Release();
	void Update();
	void HandleInput();
	void Render();

private:

};

class UIScene : public Subscene
{
public:
	UIScene(ViewManager* vm);
	void Release();
	void Update();
	void HandleInput();
	void Render();

private:
	ViewManager* VM;
};

#endif