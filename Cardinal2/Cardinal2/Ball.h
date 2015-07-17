#pragma once

#include "Misc.h"
#include "Character.h"

enum COLL
{
	OBJ, BALL, HIT,
	PASS, BOUNCE
};

class IBall
{
public:
	IBall(ICharacter* player) { parent = player; }
	~IBall()
	{
		delete parent;
		delete pEffectBase;
		delete pEffectEnd;
	}
	virtual bool Update() = 0;
	virtual COLL Hit(COLL type, ICharacter* hit) = 0;
	
	ICharacter* parent;
	D3DXVECTOR3 pos;
	double x;
	double y;
	double vx;
	double vy;
	double radius;
	double radian;

	EffectList* pEffectBase = NULL;
	EffectList* pEffectEnd = NULL;
};

typedef LList<IBall> BallList;

class Shamoo_Attack : public IBall
{
public:
	Shamoo_Attack(ICharacter* player);
};

class Shamoo_Skill : public IBall
{
public:

};

class Shamoo_Ult : public IBall
{

};