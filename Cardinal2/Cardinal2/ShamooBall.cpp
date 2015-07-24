#include "stdafx.h"
#include "Ball.h"
#include "Character.h"

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
	if (ticks >= 400)
		return false;
	circle->Step();
	ticks++;
	return true;
}

COLL Shamoo_Skill::Hit(COLL type, ICharacter* hit)
{
	switch (type)
	{
	case COLL::OBJ:
	case COLL::HIT:
		ticks = 400;
		return COLL::BOUNCE;
	default:
		return COLL::PASS;
	}
}