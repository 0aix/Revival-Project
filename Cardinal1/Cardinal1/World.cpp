#include "World.h"
#include "Constants.h"
#include "Sprite.h"

World::World(int width, int height, int cap)
{
	gridsize.x = width;
	gridsize.y = height;
	capacity = cap;
	players = new Character*[cap];
	grid = new Tile*[width];
	for (int i = 0; i < width; i++)
	{
		grid[i] = new Tile[height];
		for (int n = 0; n < height; n++)
		{
			grid[i][n].type = 4; //just which tile to use
			grid[i][n].flags = 0;
		}
	}
	size = D3DXVECTOR3(width * C::METER, height * C::METER, 0.0f); //replace w/ Constants::TILE_LENGTH later
	pBallBase = new BallList;
	pBallBase->pBall = NULL;
	pBallBase->pNext = NULL;
	pEffectBase = new EffectList;
	pEffectBase->pEffect = NULL;
	pEffectBase->pNext = NULL;
}

World::~World()
{
	for (int i = 0; i < capacity; i++)
		delete players[i];
	delete players;

	int width = gridsize.x;
	for (int i = 0; i < width; i++)
		delete grid[i];
	delete grid;
}

void World::Update(float time)
{
	for (int i = 0; i < capacity; i++)
		if (players[i] != NULL)
			players[i]->Update(time);

	BallList* prev = pBallBase;
	BallList* curr = pBallBase->pNext;
	while (curr != NULL)
	{
		Ball* ball = curr->pBall;
		if (!ball->Update(time))
		{
			//implode
			//iiiiimmmmplooooodeeee
			prev->pNext = curr->pNext;
			delete curr;
			curr = prev->pNext;
			continue;
		}
		for (int i = 0; i < capacity; i++)
		{
			if (players[i] != NULL)
			{
				Character* temp = players[i];
				float dist = ball->radius + C::RADIUS;
				float dx = abs(temp->pos.x - ball->pos.x);
				float dy = abs(temp->pos.y - ball->pos.y);
				if (dx <= dist && dy <= dist && dx * dx + dy * dy <= dist * dist)
					ball->hitfunc(temp, ball);
			}
		}
		curr = curr->pNext;
	}
	prev = pBallBase;
	curr = pBallBase->pNext;
	while (curr != NULL)
	{
		BallList* curr2 = curr->pNext;
		while (curr2 != NULL)
		{
			Ball* ball = curr->pBall;
			Ball* ball2 = curr2->pBall;
			if (ball->player != ball2->player)
			{
				float dist = ball->radius + ball2->radius;
				float dx = abs(ball2->pos.x - ball->pos.x);
				float dy = abs(ball2->pos.y - ball->pos.y);
				if (dx <= dist && dy <= dist && dx * dx + dy * dy <= dist * dist)
				{
					ball->colfunc(ball2, ball);
					ball2->colfunc(ball, ball2);
				}
			}
			curr2 = curr2->pNext;
		}
		curr = curr->pNext;
	}
	EffectList* prevE = pEffectBase;
	EffectList* currE = pEffectBase->pNext;
	while (currE != NULL)
	{
		Effect* effect = currE->pEffect;
		int res = effect->animation.Update(time);
		if (res == -1)
		{
			prevE->pNext = currE->pNext;
			delete currE;
			currE = prevE->pNext;
			continue;
		}
		else if (res == 1)
			effect->sprite = effect->animation.Render();

		currE = currE->pNext;
	}
}

bool World::Occupied(int x, int y)
{
	return grid[x][y].flags & 0x01;
}

bool World::TBOpen(int x, int y)
{
	return grid[x][y].flags & 0x02;
}

bool World::TBOccupied(int x, int y)
{
	return grid[x][y].flags & 0x04;
}

bool World::Wall(int x, int y)
{
	return grid[x][y].flags & 0x08;
}

bool World::ObjWall(int x, int y)
{
	return grid[x][y].flags & 0x10;
}

bool World::CanMoveTo(int x, int y)
{
	return (grid[x][y].flags & 0x0D) == 0;
}

void World::SetOccupied(int x, int y, bool b)
{
	if (b)
		grid[x][y].flags |= 0x01;
	else
		grid[x][y].flags &= 0xFE;
}

void World::SetTBOpen(int x, int y, bool b)
{
	if (b)
		grid[x][y].flags |= 0x02;
	else
		grid[x][y].flags &= 0xFD;
}

void World::SetTBOccupied(int x, int y, bool b)
{
	if (b)
		grid[x][y].flags |= 0x04;
	else
		grid[x][y].flags &= 0xFB;
}

void World::SetWall(int x, int y, bool b)
{
	if (b)
		grid[x][y].flags |= 0x08;
	else
		grid[x][y].flags &= 0xF7;
}

void World::SetObjWall(int x, int y, bool b)
{
	if (b)
		grid[x][y].flags |= 0x10;
	else
		grid[x][y].flags &= 0xEF;
}

Character* World::At(int x, int y)
{
	for (int i = 0; i < capacity; i++)
	{
		Character* player = players[i];
		if (player && player->cTile.x == x && player->cTile.y == y)
			return player;
	}
	return NULL;
}

void World::AddBall(Ball* ball)
{
	BallList* node = new BallList;
	node->pBall = ball;
	node->pNext = pBallBase->pNext;
	pBallBase->pNext = node;
}

void World::AddEffect(Effect* effect)
{
	EffectList* node = new EffectList;
	node->pEffect = effect;
	node->pNext = pEffectBase->pNext;
	pEffectBase->pNext = node;
}