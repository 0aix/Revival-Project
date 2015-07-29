#include "stdafx.h"
#include "Ball.h"
#include "Character.h"

Shamoo_Attack::Shamoo_Attack(ICharacter* player)
{
	parent = player;
	double radian = player->radian;
	double cosrad = cos(radian);
	double sinrad = sin(radian);
	radius = player->circle.radius;
	Vec2 pos = player->circle.pos;
	circle = new Circle(pos.x + cosrad * radius, pos.y + sinrad * radius, cosrad * 8.0, sinrad * 8.0, 60.0, 0.0, 0.0);
	circle->vel += player->circle.vel;
}

Shamoo_Attack::~Shamoo_Attack()
{
	delete circle;
}

bool Shamoo_Attack::Update()
{
	if (ticks <= 0)
		return false;
	double radian = parent->radian;
	double cosrad = cos(radian);
	double sinrad = sin(radian);
	Vec2 pos = parent->circle.pos;
	circle->pos.x = pos.x + cos(radian) * radius;
	circle->pos.y = pos.y + sin(radian) * radius;
	circle->vel = Vec2(cosrad * 8.0, sinrad * 8.0) + parent->circle.vel;
	ticks--;
	return true;
}

COLL Shamoo_Attack::Hit(COLL type, ICharacter* hit)
{
	//store what character hit so you don't hit em again...
	//gen effect no matter what
	//possible to check collision here, and then resolve it... or pass the normal vector... ? (store the last normal/penetration with shape?)
	if (type == COLL::HIT)
	{
		hit->Hit(20.0, 0.05);
		return COLL::BOUNCE;
	}
	return COLL::PASS;
}

Shamoo_Skill::Shamoo_Skill(ICharacter* player)
{
	parent = player;
	double radian = player->radian;
	double radius = player->circle.radius;
	double cosrad = cos(radian);
	double sinrad = sin(radian);
	Vec2 pos = player->circle.pos;
	double speed = 800.0 * C::TICK;
	circle = new Circle(pos.x + cosrad * radius, pos.y + sinrad * radius, cosrad * speed, sinrad * speed, 20.0, 1.0 / 800.0, 1.0);
}

Shamoo_Skill::~Shamoo_Skill()
{
	delete circle;
}

bool Shamoo_Skill::Update()
{
	if (ticks <= 0)
		return false;
	circle->Step();
	ticks--;
	return true;
}

COLL Shamoo_Skill::Hit(COLL type, ICharacter* hit)
{
	switch (type)
	{
	case COLL::OBJ:
		ticks = 0;
		return COLL::BOUNCE;
	case COLL::HIT:
		ticks = 0;
		hit->Hit(20.0, 0.05);
		return COLL::BOUNCE;
	default:
		return COLL::PASS;
	}
}

Shamoo_Ult::Shamoo_Ult(ICharacter* player)
{
	parent = player;
	double radian = player->radian;
	double radius = player->circle.radius;
	double cosrad = cos(radian);
	double sinrad = sin(radian);
	Vec2 pos = player->circle.pos;
	double speed = 1600.0 * C::TICK;
	circle = new Circle(pos.x + cosrad * radius, pos.y + sinrad * radius, cosrad * speed, sinrad * speed, 80.0, 1.0 / 1600.0, 1.0);
}

Shamoo_Ult::~Shamoo_Ult()
{
	delete circle;
}

bool Shamoo_Ult::Update()
{
	if (ticks <= 0)
		return false;
	circle->Step();
	ticks--;
	return true;
}

COLL Shamoo_Ult::Hit(COLL type, ICharacter* hit)
{
	switch (type)
	{
	case COLL::OBJ:
		ticks = 0;
		return COLL::BOUNCE;
	case COLL::HIT:
		ticks = 0;
		hit->Hit(40.0, 0.05);
		return COLL::BOUNCE;
	default:
		return COLL::PASS;
	}
}