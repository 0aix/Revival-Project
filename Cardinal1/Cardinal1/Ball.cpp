#include "World.h"
#include "Constants.h"

Ball::Ball(Character* c, World* map, D3DXVECTOR3 off, POINT t, float r, float d, int hp, bool col, bool p, Param param, Call hit, Coll coll)
{
	player = c;
	world = map;
	offset = off;
	tile = t;
	fpos = D3DXVECTOR3(t.x * C::METER, t.y * C::METER, 0.0f);
	radius = r;
	duration = d;
	hits = hp;
	collision = col;
	pierce = p;
	acc = 0.0f;
	parfunc = param;
	hitfunc = hit;
	colfunc = coll;
	parfunc(this);
}

bool Ball::Update(float time)
{
	if (acc > duration)
		return false;

	acc += time;
	if (acc > duration)
		acc = duration;

	parfunc(this);
	if (collision)
	{
		float dx = fpos.x - pos.x;
		float dy = fpos.x - pos.y;
		if (dx < 0)
		{
			tile.x -= 1;
			fpos.x -= C::METER;
		}
		else if (dx >= C::METER)
		{
			tile.x += 1;
			fpos.x += C::METER;
		}
		if (dy < 0)
		{
			tile.y -= 1;
			fpos.y -= C::METER;
		}
		else if (dy >= C::METER)
		{
			tile.y += 1;
			fpos.y += C::METER;
		}
		if (world->ObjWall(tile.x, tile.y))
			return false;
	}
	return true;
}