#include "stdafx.h"
#include "Subscene.h"
#include "World.h"

WorldScene::WorldScene() : ISubscene()
{
	player = new Shamoo();
	world = new World(player, 8192.0, 8192.0, 2);
	player->circle.pos.x = 4096.0;
	player->circle.pos.y = 4096.0;
	world->players[1] = new Shamoo();
	world->players[1]->circle.pos.x = 4096.0;
	world->players[1]->circle.pos.y = 4096.0;
	camera.x = 4096.0 - C::CENTER_X;
	camera.y = 4096.0 - C::CENTER_Y;
	if (camera.x < 0.0f)
		camera.x = 0.0f;
	if (camera.y < 0.0f)
		camera.y = 0.0f;

	circle = (IDirect3DTexture9*)Cardinal::GetState(0);
	arrow = (IDirect3DTexture9*)Cardinal::GetState(1);
	tile = (IDirect3DTexture9*)Cardinal::GetState(2);

	scaling = D3DXVECTOR2(C::RATIO, C::RATIO);
	D3DXMatrixTransformation2D(&scalingmat, NULL, 0.0f, &scaling, NULL, 0.0f, NULL);

	dTime = 0.0;
	clock.Start();
}

void WorldScene::Release()
{
	tile->Release();
	arrow->Release();
	circle->Release();
	delete world;
	delete player;
	delete this;
}

void WorldScene::Update() 
{
	dTime += clock.Time();
	while (dTime >= C::TICK)
	{
		world->Update();
		camera.x = player->circle.pos.x - C::CENTER_X;
		camera.y = player->circle.pos.y - C::CENTER_Y;
		if (camera.x < 0.0f)
			camera.x = 0.0f;
		if (camera.y < 0.0f)
			camera.y = 0.0f;
		if (camera.x > 6144.0f)
			camera.x = 6144.0f;
		if (camera.y > 6656.0f)
			camera.y = 6656.0f;

		dTime -= C::TICK;
	}
}

void WorldScene::HandleInput() 
{
	//?
	if (pInput->CLR)
	{
		//player->Nop();
	}

	//
	if (pInput->type == 1) //Keyboard
	{
		byte keycode = pInput->keycode;
		bool state = pInput->state;
		if (keycode == 'A')
			player->Key(S::BOOSTER, state);
		else if (keycode == VK_UP)
			player->Key(S::FORWARD, state);
		else if (keycode == VK_DOWN)
			player->Key(S::BACK, state);
		else if (keycode == VK_LEFT)
			player->Key(S::CCW, state);
		else if (keycode == VK_RIGHT)
			player->Key(S::CW, state);
	}
}

void WorldScene::Render() 
{
	//Do as much of the batch with the default transform matrix
	sprite->SetTransform(&scalingmat);

	D3DXVECTOR3 pos((float)(floor(camera.x / C::METER) * C::METER), (float)(floor(camera.y / C::METER) * C::METER), 0.0f);
	pos -= camera;
	sprite->Draw(tile, NULL, NULL, &pos, 0xFFFFFFFF);

	int capacity = world->capacity;
	ICharacter** players = world->players;
	D3DXVECTOR3 center(128.0f, 128.0f, 0.0f);
	for (int i = 0; i < capacity; i++)
	{
		ICharacter* player = players[i];
		if (player != NULL)
		{
			pos = player->pos - center - camera;
			sprite->Draw(circle, NULL, NULL, &pos, 0xFFFFFFFF);
		}
	}

	//Do the rest of the batch that needs rotated matrices
	D3DXVECTOR2 center2d = D3DXVECTOR2(-112.5f, 37.5f);
	D3DXMATRIX mat;
	for (int i = 0; i < capacity; i++)
	{
		ICharacter* player = players[i];
		if (player != NULL)
		{
			D3DXVECTOR2 pos2d(player->pos.x - camera.x, player->pos.y - camera.y);
			Graphics::D3DX2DTransform(&mat, &center2d, player->radian, &pos2d, &scaling);
			sprite->SetTransform(&mat);
			sprite->Draw(arrow, NULL, NULL, NULL, 0xFFFFFFFF);
		}
	}
}