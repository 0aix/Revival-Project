#pragma once

#include <d3dx9.h>

enum S
{
	NOP, CCW, CW, 
	FORE, BACK,
	ATTACK, SKILL, ULT, 
	KB, STUN, GUARD
};

class ICharacter
{
public:
	virtual void Update() = 0;
	virtual void Keydown(byte state) = 0;
	virtual void Keyup(byte state) = 0;
	virtual void Attack() = 0;
	virtual void Skill() = 0;
	virtual void Ult() = 0;
	virtual void Guard() = 0;

	D3DXVECTOR3 pos;
	double speed;
	double acc;
	double rotation;
	double angle;

	byte state;
	byte rstate; //0 - nope, 1 - ccw, 2 - cw
	double lock;

	double hp;
	double sp;
	double mp;
	double spregen;
	double mpregen;

};

class Shamoo : public ICharacter
{
public:
	Shamoo();
	void Update();
	void Keydown(byte state);
	void Keyup(byte state);
	void Attack();
	void Skill();
	void Ult();
	void Guard();
};