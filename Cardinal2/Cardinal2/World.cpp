#include "stdafx.h"
#include "World.h"

#define BOUNCE -0.5

World::World(ICharacter* character, double w, double h, int cap)
{
	players = new ICharacter*[cap] { 0 };
	for (int i = 0; i < cap; i++)
		players[i] = NULL;
	players[0] = character;
	width = w;
	height = h;
	capacity = cap;

	Line L[4];
	L[0] = Line(0.0, 0.0, 0.0, h);
	L[1] = Line(0.0, h, w, h);
	L[2] = Line(w, h, w, 0.0);
	L[3] = Line(w, 0.0, 0.0, 0.0);
	box = new Box(L, 4, 0.0, 0.0, 0.0, 1.0);
}

World::~World()
{
	delete[] players;
}

//Set Z for everything
void World::Update()
{
	for (int i = 0; i < capacity; i++)
	{
		ICharacter* player = players[i];
		if (player)
		{
			player->Update();
			Circle& circle = player->circle;
			Collision coll;
			for (int i = box->count - 1; i >= 0; i--)
			{
				if (Physics::CircleToLine(circle, box->S[i], &coll))
					Physics::ResolveCollision(circle, *box, coll);
			}
			for (int n = i; n < capacity; n++)
			{
				ICharacter* player2 = players[n];
				if (player2)
				{
					Circle& circle2 = player2->circle;
					if (Physics::CircleToCircle(circle, circle2, &coll))
						Physics::ResolveCollision(circle, circle2, coll);
				}
			}
		}
	}
}