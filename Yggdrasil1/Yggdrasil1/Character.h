#pragma once

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
	virtual void Update() = 0;
	virtual void Key(S action, bool down) = 0;
	virtual void Attack() = 0;
	virtual void Skill() = 0;
	virtual void Ult() = 0;
	virtual void Shield() = 0;
	virtual void Neutral() = 0;
	virtual void Hit(double dmg, double stun) = 0;
	virtual void Die() = 0;

	Circle circle;
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

	BallList ballList;
	EffectList effectList;
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
	void Hit(double dmg, double stun);
	void Die();
};