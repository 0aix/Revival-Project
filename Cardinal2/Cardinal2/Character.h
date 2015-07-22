#pragma once

#include <d3dx9.h>
#include "Misc.h"
#include "Physics.h"

class IBall;
struct Effect;
typedef LList<IBall> BallList;
typedef LList<Effect> EffectList;

enum S
{
	NOP, CCW, CW, 
	BOOSTER, FORWARD, BACK,
	ATTACK, SKILL, ULT, SHIELD,
	ATTCAST, SKILLCAST, ULTCAST,
	KB, STUN, DEAD
};

class ICharacter
{
public:
	~ICharacter()
	{
		delete pEffectBase;
		delete pEffectEnd;
	}
	virtual void Update() = 0;
	virtual void Key(S action, bool down) = 0;
	virtual void Attack() = 0;
	virtual void Skill() = 0;
	virtual void Ult() = 0;
	virtual void Shield() = 0;
	virtual void Neutral() = 0;
	virtual void Hit(double dmg, bool stun, double time) = 0;
	virtual void Die() = 0;

	D3DXVECTOR3 pos;
	Circle circle;
	double speed;
	double accel;
	double angular; //angular speed
	double radian; //direction
	double maxspeed;

	S state;
	S mstate; //nope, fore, back
	S rstate; //nope, ccw, cw
	bool canMove;
	int lock;
	int buffer;
	int cast;
	S next;

	double hp;
	double sp;
	double mp;
	double gp;
	double hpregen;
	double spregen;
	double mpregen;
	double gpregen;

	//BallList* pBallBase = NULL;
	EffectList* pEffectBase = NULL;
	EffectList* pEffectEnd = NULL;
};

class Shamoo : public ICharacter
{
public:
	Shamoo();
	void Update();
	void Key(S action, bool down);
	void Attack();
	void Skill();
	void Ult();
	void Shield();
	void Neutral();
	void Hit(double dmg, bool stun, double time);
	void Die();
};