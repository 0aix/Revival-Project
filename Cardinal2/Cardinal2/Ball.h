#pragma once

#include "Misc.h"

class ICharacter;
struct Circle;
struct Box;

enum COLL
{
	OBJ, BALL, HIT,
	PASS, BOUNCE
};

class IBall
{
public:
	IBall(ICharacter* player);
	~IBall();
	virtual bool Update() = 0;
	virtual COLL Hit(COLL type, ICharacter* hit) = 0;
	
	//ONLY ONE HITBOX IS ALLOWED; EITHER THE CIRCLE OR THE BOX
	ICharacter* parent;
	D3DXVECTOR3 pos;
	Circle* circle = NULL;
	Box* box = NULL;

	EffectList effectList;
};

typedef LList<IBall> BallList;

class Shamoo_Attack : public IBall
{
public:
	Shamoo_Attack(ICharacter* player);
	~Shamoo_Attack();
	bool Update();
	COLL Hit(COLL type, ICharacter* hit);

	double radius;
	int ticks = 50;
};

class Shamoo_Skill : public IBall
{
public:
	Shamoo_Skill(ICharacter* player);
	~Shamoo_Skill();
	bool Update();
	COLL Hit(COLL type, ICharacter* hit);

	int ticks = 400;
};

class Shamoo_Ult : public IBall
{
public:
	Shamoo_Ult(ICharacter* player);
	~Shamoo_Ult();
	bool Update();
	COLL Hit(COLL type, ICharacter* hit);

	int ticks = 400;
};