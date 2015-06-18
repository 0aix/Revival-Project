#include "Subscene.h"
#include "World.h"

WorldScene::WorldScene(Character* c, World* w) : Subscene()
{
	player = c;
	world = w;
	players = world->players;
	pBallBase = world->pBallBase;
	pEffectBase = world->pEffectBase;
	//set camera
	camera.x = player->pos.x - C::CENTER_X;
	camera.y = player->pos.y - C::CENTER_Y;
	if (camera.x < 0.0f)
		camera.x = 0.0f;
	if (camera.y < 0.0f)
		camera.y = 0.0f;
	
	D3DXMATRIX mat;
	D3DXVECTOR2 scaling = D3DXVECTOR2(C::RATIO, C::RATIO);
	D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &scaling, NULL, 0.0f, NULL);
	sprite->SetTransform(&mat);


	Clock.Start();
}

void WorldScene::Release()
{
	delete this;
}

void WorldScene::Update()
{
	fTime += Clock.Time();
	while (fTime >= C::TICK)
	{
		world->Update(C::TICK);
		//update camera .. do this better....
		float rx = player->pos.x - camera.x;
		float ry = player->pos.y - camera.y;
		if (rx > C::CENTER_X)
		{
			//make it nonlinear?
			if (rx - C::CENTER_X < C::CAMERA_SCROLL * C::TICK)
				camera.x += rx - C::CENTER_X;
			else
				camera.x += C::CAMERA_SCROLL * C::TICK;
		}
		else if (rx < C::CENTER_X)
		{
			if (C::CENTER_X - rx < C::CAMERA_SCROLL * C::TICK)
				camera.x -= C::CENTER_X - rx;
			else
				camera.x -= C::CAMERA_SCROLL * C::TICK;
		}
		if (ry > C::CENTER_Y)
		{
			if (ry- C::CENTER_Y < C::CAMERA_SCROLL * C::TICK)
				camera.y += ry - C::CENTER_Y;
			else
				camera.y += C::CAMERA_SCROLL * C::TICK;
		}
		else if (ry < C::CENTER_Y)
		{
			if (C::CENTER_Y - ry < C::CAMERA_SCROLL * C::TICK)
				camera.y -= C::CENTER_Y - ry;
			else
				camera.y -= C::CAMERA_SCROLL * C::TICK;
		}
		if (camera.x < 0.0f)
			camera.x = 0.0f;
		else if (camera.x > world->size.x - C::SCREEN_WIDTH) //set variable later..
			camera.x = world->size.x - C::SCREEN_WIDTH;
		if (camera.y < 0.0f)
			camera.y = 0.0f;
		else if (camera.y > world->size.y - C::SCREEN_HEIGHT)
			camera.y = world->size.y - C::SCREEN_HEIGHT;
		//
		fTime -= C::TICK;
	}
}

void WorldScene::HandleInput()
{

}

void WorldScene::Render()
{
	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	//map tiles




	//need to render players depending on z
	int cap = world->capacity;
	for (int i = 0; i < cap; i++)
	{
		if (players[i] != NULL)
		{
			Sprite* sp = players[i]->sprite;
			D3DXVECTOR3 rpos = players[i]->pos - sp->center - camera;
			sprite->Draw(sp->texture, NULL, NULL, &rpos, 0xFFFFFFFF);
		}
	}

	//balls

	//effects
	EffectList* prevE = pEffectBase;
	EffectList* currE = pEffectBase->pNext;
	while (currE != NULL)
	{
		Effect* effect = currE->pEffect;
		Sprite* sp = effect->sprite;
		D3DXVECTOR3 rpos = (effect->pos - sp->center - camera) * C::RATIO;
		sprite->Draw(sp->texture, NULL, NULL, &rpos, 0xFFFFFFFF);
		currE = currE->pNext;
	}

	sprite->End();
}