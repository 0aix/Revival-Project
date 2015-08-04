#include "stdafx.h"
#include "Subscene.h"
#include "World.h"

WorldScene::WorldScene(int width, int length, int cap) : ISubscene()
{
	double dwidth = width * C::METER;
	double dlength = length * C::METER;
	player = new Shamoo();
	world = new World(player, dwidth, dlength, cap);
	double hwidth = dwidth / 2.0;
	double hlength = dlength / 2.0;
	player->circle.pos.x = hwidth;
	player->circle.pos.y = hlength;
	world->players[1] = new Shamoo();
	world->players[1]->circle.pos.x = hwidth;
	world->players[1]->circle.pos.y = hlength;
	camera.x = hwidth - C::CENTER_X;
	camera.y = hlength - C::CENTER_Y;
	xbound = dwidth - C::SCREEN_WIDTH;
	ybound = dlength - C::SCREEN_HEIGHT;
	//if (camera.x < 0.0f)
	//	camera.x = 0.0f;
	//if (camera.y < 0.0f)
	//	camera.y = 0.0f;

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

		//PULLING/PUSHING CAMERA IMPLEMENTATION annoying
		//this camera implementation sucks ass. do not use.
		//if hit, switch camera method to screen pushing
		/*
		Circle circle = player->circle;
		double vx = circle.vel.x;
		double vy = circle.vel.y;
		double cx = camera_offset.x;
		double cy = camera_offset.y;
		double cspd = 1.0;
		double xlimit = C::CENTER_X / 1.618;
		double ylimit = C::CENTER_Y / 1.618;
		if (player->canMove && player->mstate != S::NOP) //accel
		{
			if (cx != 0.0 || cy != 0.0) //perp. slide
			{
				Vec2 vel(vx, vy);
				vel.Normalize();
				Vec2 offset(cx, cy);
				double dot = offset * vel;
				if (dot > 0)
					offset -= vel * dot;
				offset.Normalize();
				offset *= cspd;
				if (cx > 0.0)
					cx = max(0.0, cx - offset.x);
				else if (cx < 0.0)
					cx = min(0.0, cx - offset.x);
				if (cy > 0.0)
					cy = max(0.0, cy - offset.y);
				else if (cy < 0.0)
					cy = min(0.0, cy - offset.y);
			}
			double ox = cx;
			double oy = cy;
			cx += vx; //* cspd?
			cy += vy; //* cspd?
			if (vx != 0.0 || vy != 0.0)
			{
				if (cx > xlimit || cx < -xlimit)
					cx = cx > 0.0 ? xlimit : -xlimit;
				if (cy > ylimit || cy < -ylimit)
					cy = cy > 0.0 ? ylimit : -ylimit;
				double tx = vx != 0.0 ? abs(xlimit / vx) : -1.0;
				double ty = vy != 0.0 ? abs(ylimit / vy) : -1.0;
				if (tx >= 0.0 && tx < ty)
				{
					double ny = vy / abs(vx) * xlimit;
					double dy1 = cy - ny;
					double dy2 = oy - ny;
					if (signbit(dy1) ^ signbit(dy2))
						cy = ny;
					else
						cy = abs(dy1) < abs(dy2) ? cy : oy;
				}
				else if (ty >= 0.0 && ty < tx)
				{
					double nx = vx / abs(vy) * ylimit;
					double dx1 = cx - nx;
					double dx2 = ox - nx;
					if (signbit(dx1) ^ signbit(dx2))
						cx = nx;
					else
						cx = abs(dx1) < abs(dx2) ? cx : ox;
				}
			}
		}
		else if (player->state == S::KB) //kb
		{

		}
		else if (cx != 0.0 || cy != 0.0) //slide
		{
			if (cx * cx + cy * cy > 10000.0)
			{
				double radian = atan2(cy, cx);
				double dx = cos(radian) * cspd;
				double dy = sin(radian) * cspd; //cy / cx * dx; tech faster.. right?
				if (cx > 0.0)
				cx = max(0.0, cx - dx);
				else if (cx < 0.0)
				cx = min(0.0, cx - dx);
				if (cy > 0.0)
					cy = max(0.0, cy - dy);
				else if (cy < 0.0)
					cy = min(0.0, cy - dy);
				//cx *= 0.99;
				//cy *= 0.99;
			}
		}
		
		camera_offset.x = cx;
		camera_offset.y = cy;
		*/
		//comment out above and camera_offset for locked screen

		camera.x = player->circle.pos.x - C::CENTER_X;
		camera.y = player->circle.pos.y - C::CENTER_Y;
		//camera += camera_offset;
		if (camera.x < 0.0f)
		{
			//camera_offset.x -= camera.x;
			camera.x = 0.0f;
		}
		if (camera.y < 0.0f)
		{
			//camera_offset.y -= camera.y;
			camera.y = 0.0f;
		}
		if (camera.x > xbound)
		{
			//camera_offset.x -= camera.x - xbound;
			camera.x = xbound;
		}
		if (camera.y > ybound)
		{
			//camera_offset.y -= camera.y - ybound;
			camera.y = ybound;
		}

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
		bool state = pInput->state;
		switch (pInput->keycode)
		{
		case 'A':
			player->Key(S::BOOSTER, state);
			break;
		case 'S':
			player->Key(S::ATTACK, state);
			break;
		case 'D':
			player->Key(S::SKILL, state);
			break;
		case 'R':
			player->Key(S::ULT, state);
			break;
		case VK_UP:
			player->Key(S::FORWARD, state);
			break;
		case VK_DOWN:
			player->Key(S::BACK, state);
			break;
		case VK_LEFT:
			player->Key(S::CCW, state);
			break;
		case VK_RIGHT:
			player->Key(S::CW, state);
			break;
		}
		/*byte keycode = pInput->keycode;
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
			player->Key(S::CW, state);*/
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
	//right now shows everyone; remove this later
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