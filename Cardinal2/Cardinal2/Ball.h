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
	~IBall()
	{
		//change this to ACTUALLY DELETE STUFF
		delete pEffectBase;
		delete pEffectEnd;
	}
	virtual bool Update() = 0;
	virtual COLL Hit(COLL type, ICharacter* hit) = 0;
	
	//ONLY ONE HITBOX IS ALLOWED; EITHER THE CIRCLE OR THE BOX
	D3DXVECTOR3 pos;
	Circle* circle = NULL;
	Box* box = NULL;

	EffectList* pEffectBase = NULL;
	EffectList* pEffectEnd = NULL;
};

typedef LList<IBall> BallList;

class Shamoo_Attack : public IBall
{
public:

};

class Shamoo_Skill : public IBall
{
public:
	Shamoo_Skill(ICharacter* player);
	~Shamoo_Skill();
	bool Update();
	COLL Hit(COLL type, ICharacter* hit);

	ICharacter* parent;
	int ticks = 0;
};

class Shamoo_Ult : public IBall
{

};