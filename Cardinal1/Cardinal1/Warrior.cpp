#include "World.h"
#include "Constants.h"

Warrior::Warrior(World* map, int tx, int ty, char* cname) : Character(map)
{
	cTile.x = tx;
	cTile.y = ty;
	nTile = cTile;
	pos = D3DXVECTOR3(tx * C::METER + C::HALF, ty * C::METER + C::HALF, 0.0f);
	npos = pos;
	vel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	state = S::Stand;
	dir = S::Down;

	maxhp = 1000.0f; hp = 1000.0f;
	maxmp = 100.0f; mp = 100.0f;
	maxsp = 500.0f; sp = 500.0f;
	hpr = 10.0f; mpr = 10.0f; spr = 10.0f;

	cooldown[0] = 0.0f; cooldown[1] = 0.0f; cooldown[2] = 0.0f; cooldown[3] = 0.0f;

	speed = 400.0f;
	rollspeed = 500.0f;
	cast = 0.0f;
	locked = 0.0f;
	nextaction = S::Stand;
	queue = 0.0f;
	autochain = 0.0f;
	lastauto = 0;
	presstime = 0.0f;
	xstate = 0;
	tilelock = true;
	roll = false;
	keylock = false;
	guard = false;
	
	name = cname;
	kills = 0;
	deaths = 0;

	//ANIMS HERE
	Anim* anim;
	while (!Jotunheimr::LoadResource(TO::ANIM, TO::WARRIOR_STAND_D, (void**)&anim));
	animation = SpriteAnimation(anim->spriteanim);
	sprite = animation.Render();
}

void Warrior::Attack()
{
	if (autochain > 0.0f)
	{
		if (lastauto == 0)
		{
			locked = C::WARRIOR_AUTO2;
			autochain = C::WARRIOR_AUTO2 + C::WARRIOR_AUTO_CHAIN;
		}
		else
		{
			locked = C::WARRIOR_AUTO3;
			autochain = 0.0f;
			lastauto = 0;
		}
		state = S::Attack;
		nextaction = S::Stand;
		//queue = 0.0f;
	}
	else
	{
		locked = C::WARRIOR_AUTO1; //should i give it a cast to cancel?
		//queue = 0.0f;
		autochain = C::WARRIOR_AUTO1 + C::WARRIOR_AUTO_CHAIN;
		state = S::Attack;
		nextaction = S::Stand;
		lastauto = 0;
	}
}

//we're gonna go w/ cast for this one
//handled it in original Update, we can move it i guess if i feel like it
void Warrior::Guard()
{
	state = S::Guard;
	nextaction = S::Guard;
	cast = C::WARRIOR_GUARD;
	locked = 0.0f;
	//queue = 0.0f;
	//doesn't break autochain cuz sweet combos maybe?
}

//Crosscut - Wide, 2x cast
void Warrior::Skill1()
{

}

/*
Charge into Shield Bash - Dash forward; stun; increased defense
into Attack/Skill
- Guard + Charge casts Shield Bash
- Charge by itself knocks back
*/
void Warrior::Skill2()
{

}

//Assault Slash - Jump forward and slash downward + AoE quake
void Warrior::Skill3()
{

}

//Provoke - AoE Slow + Insta-Pull
void Warrior::Skill4()
{

}

//handle cast times in here
void Warrior::UpdateA(float time)
{
	
}

bool Warrior::Hit(int direction, int type)
{
	return true;
}