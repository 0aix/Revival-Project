#include "stdafx.h"
#include "World.h"
#include "Character.h"

#define BOUNCE -0.5

SquareWorld::SquareWorld(ICharacter* character, double length, int cap) : IWorld(character, length, length, cap)
{

}

SquareWorld::~SquareWorld()
{

}

//Set Z for everything
void SquareWorld::Update()
{
	for (int i = 0; i < capacity; i++)
	{
		ICharacter* player = players[i];
		if (player)
		{
			player->Update();
			double x = player->x;
			double y = player->y;
			double vx = player->vx;
			double vy = player->vy;
			double dirx = player->accel * cos(player->radian);
			double diry = player->accel * sin(player->radian);
			bool moving = player->canMove && player->mstate != S::NOP;
			bool update = false;
			//Assume map is wider than the players...
			if (x - C::RADIUS < 0.0)
			{
				if (vx < 0.0)
				{
					if (!moving || dirx >= 0.0)
						vx *= BOUNCE;
					else
						vx *= BOUNCE / 2.0;
					player->vx = vx;
					player->x = C::RADIUS;
				}
				update = true;
			}
			else if (x + C::RADIUS > width)
			{
				if (vx > 0.0)
				{
					if (!moving || dirx <= 0.0)
						vx *= BOUNCE;
					else
						vx *= BOUNCE / 2.0;
					player->vx = vx;
					player->x = width - C::RADIUS;
				}
				update = true;
			}
			if (y - C::RADIUS < 0.0)
			{
				if (vy < 0.0)
				{
					if (!moving || diry >= 0.0)
						vy *= BOUNCE;
					else
						vy *= BOUNCE / 2.0;
					player->vy = vy;
					player->y = C::RADIUS;
				}
				update = true;
			}
			else if (y + C::RADIUS > height)
			{
				if (vy > 0.0)
				{
					if (!moving || diry <= 0.0)
						vy *= BOUNCE;
					else
						vy *= BOUNCE / 2.0;
					player->vy = vy;
					player->y = height - C::RADIUS;
				}
				update = true;
			}
			if (update)
				player->speed = sqrt(vx * vx + vy * vy);
		}
	}
}