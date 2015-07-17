#include "stdafx.h"
#include "Character.h"
#include "Ball.h"
#include "Sprite.h"

#define MAX_HP 500.0
#define MAX_SP 100.0
#define MAX_MP 500.0
#define MAX_GP 100.0
#define MAX_SPD 2200.0 * C::TICK
#define ANG_SPD 1.1 * C::PI * C::TICK
#define MASS 800.0
#define MU 1600.0 * C::TICK * C::TICK
#define BOOST 4096.0 * C::TICK * C::TICK
#define WALK 2000.0 * C::TICK * C::TICK
#define WALK_SPD 1000.0 * C::TICK

//calc/define variables based on tick.
Shamoo::Shamoo()
{
	pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	x = 0.0; y = 0.0;
	vx = 0.0; vy = 0.0;
	speed = 0.0;
	accel = 0.0;
	angular = 0.0;
	radian = 0.0;
	maxspeed = 0.0;
	//mass = 800.0;
	//friction = mass * C::TICK * C::TICK;

	state = S::NOP;
	mstate = S::NOP;
	rstate = S::NOP;
	canMove = true;
	lock = -1;
	buffer = -1;
	cast = -1;

	hp = MAX_HP;
	sp = MAX_SP;
	mp = MAX_MP;
	gp = MAX_GP;
	hpregen = 0.05; //per frame; 10.0 per second
	spregen = 0.2; //per frame; 40.0 per second
	mpregen = 1.25; //per frame; 250.0 per second
	gpregen = 0.05; //per frame; 10.0 per second
}

//everything goes by frame
void Shamoo::Update()
{
	//Do state updates first
	if (state != S::DEAD)
	{
		hp = min(hp + hpregen, MAX_HP);
		sp = min(sp + spregen, MAX_SP);
		mp = min(mp + mpregen, MAX_MP);
		gp = min(gp + gpregen, MAX_GP);
	}

	//DISCRETE IMPLEMENTATION
	/**/
	//Radian, Acceleration, Friction, Position
	radian += angular;
	double angle;
	if (vx == 0.0 && vy == 0.0)
		angle = radian;
	else
		angle = atan2(vy, vx);

	double cosrad = cos(radian);
	double sinrad = sin(radian);
	double mx = maxspeed * cosrad;
	double my = maxspeed * sinrad;
	double ax = accel * cosrad;
	double ay = accel * sinrad;
	double fx = MU * cos(angle);
	double fy = MU * sin(angle);
	if (vx > 0.0)
		vx = max(0.0, vx - fx);
	else if (vx < 0.0)
		vx = min(0.0, vx - fx);
	if (vy > 0.0)
		vy = max(0.0, vy - fy);
	else if (vy < 0.0)
		vy = min(0.0, vy - fy);
	
	if (canMove)
	{
		if (ax > 0.0 && vx < mx)
			vx = min(mx, vx + ax);
		else if (ax < 0.0 && vx > mx)
			vx = max(mx, vx + ax);
		if (ay > 0.0 && vy < my)
			vy = min(my, vy + ay);
		else if (ay < 0.0 && vy > my)
			vy = max(my, vy + ay);
	}
	x += vx;
	y += vy;

	pos.x = x;
	pos.y = y;
	radian = fmod(radian, C::TWOPI);

	if (lock >= 0)
		lock--;
	if (buffer >= 0)
		buffer--;
	if (cast > 0)
	{
		if (--cast == 0)
		{
			if (state == S::ATTACK)
				Attack();
			else if (state == S::SKILL)
				Skill();
			else if (state == S::ULT)
				Ult();
		}
	}

	//Handle inputs last
	if (lock < 0)
	{
		if (buffer >= 0)
		{
			if (next == S::ATTACK)
				Attack();
			else if (next == S::SKILL)
				Skill();
			else if (next == S::ULT)
				Ult();
			else if (next == S::SHIELD)
				Shield();
		}
		else
			Neutral();
	}
}

void Shamoo::Key(S action, bool down)
{
	if (action == S::BOOSTER || action == S::FORWARD || action == S::BACK)
	{
		if (down)
		{
			mstate = action;
			if (action == S::BOOSTER)
			{
				accel = BOOST;
				maxspeed = MAX_SPD;
			}
			else if (action == S::FORWARD)
			{
				accel = WALK;
				maxspeed = WALK_SPD;
			}
			else
			{
				accel = -WALK;
				maxspeed = -WALK_SPD;
			}
		}
		else if (mstate == action)
		{
			mstate = S::NOP;
			accel = 0.0;
		}
	}
	else if (action == S::CCW || action == S::CW)
	{
		if (down)
		{
			rstate = action;
			angular = action == S::CCW ? -ANG_SPD : ANG_SPD;
		}
		else if (rstate == action) //&& up
		{
			rstate = S::NOP;
			angular = 0.0;
		}
	}
}

void Shamoo::Attack()
{
	if (state == S::ATTACK)
	{
		//Time to cast...
		cast = -1;
		state = S::ATTCAST;


	}
	else if (sp >= 20.0)
	{
		cast = 80; //0.400 secs
		lock = 100;//0.500 secs
		sp -= 20.0; //subtract on cast instead?
		state = S::ATTACK;


	}
}

void Shamoo::Skill()
{
	if (state == S::SKILL)
	{
		cast = -1;
		state = S::SKILLCAST;


	}
	else if (mp >= 40.0)
	{
		cast = 20; //0.100 secs
		lock = 25;//0.125 secs
		mp -= 40.0; //subtract on cast instead?
		state = S::SKILL;


	}
}

void Shamoo::Ult()
{
	if (state == S::ULT)
	{
		cast = -1;
		state = S::ULTCAST;


	}
	else if (mp >= 250.0)
	{
		cast = 200; //1.000 secs
		lock = 240;//1.200 secs
		mp -= 250.0; //subtract on cast instead?
		state = S::ULT;


	}
}

void Shamoo::Shield()
{

}

void Shamoo::Neutral()
{

}

void Shamoo::Hit(double angle, double impulse, double dmg, bool stun, double time)
{

}

void Shamoo::Impulse(double angle, double impulse, bool cancel)
{
	if (cancel) //momentum cancel
	{
		vx = 0.0;
		vy = 0.0;
	}
	vx += impulse * cos(angle);
	vy += impulse * sin(angle);
}

void Shamoo::Die()
{

}