#include "World.h"
#include "Constants.h"

void Character::Update(float time)
{
	UpdateA(time);

	if (state <= S::DownR)
	{
		pos.x += vel.x * time;
		pos.y += vel.y * time;
		if (!tilelock && abs(npos.x - pos.x) <= C::RADIUS || abs(npos.y - pos.y) <= C::RADIUS)
		{
			world->SetTBOpen(cTile.x, cTile.y, false);
			world->SetOccupied(cTile.x, cTile.y, false);
			world->SetTBOccupied(nTile.x, nTile.y, false);
			world->SetOccupied(nTile.x, nTile.y, true);
			cTile = nTile;
			tilelock = true;
		}
		if (locked <= time)
		{
			pos = npos;
			vel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
	}

	if (locked > 0.0f)
		locked -= time;
	if (queue > 0.0f)
		queue -= time;
	if (cast > 0.0f)
		cast -= time;
	if (autochain > 0.0f)
		autochain -= time;
	if (keylock)
		presstime += time;

	for (int i = 0; i < 4; i++)
		if (cooldown[i] > 0.0f)
			cooldown[i] -= time;

	if (state == S::Guard && cast <= 0.0f)
		guard = true;

	if (S::LeftT <= state && state <= S::DownT && keylock && presstime >= C::TURN_MOVE_TIME)
	{
		nextaction = state % 4; //change to move
		Move(nextaction, false); //probably works...?
	}

	if (locked <= 0.0f)
	{
		if (queue > 0.0f)
		{
			if (nextaction <= S::Down)
				Move(nextaction, false);
			else if (nextaction <= S::DownR)
				Move(nextaction, true);
			else if (nextaction <= S::DownT)
				Turn(nextaction);
			else
			{
				switch (nextaction)
				{
				case S::Skill1:
					Skill1();
					break;
				case S::Skill2:
					Skill2();
					break;
				case S::Skill3:
					Skill3();
					break;
				case S::Skill4:
					Skill4();
					break;
				case S::Attack:
					Attack();
					break;
				case S::Guard:
					Guard();
					break;
				default:
					Stand();
				}
			}
		}
		else if (state != S::Stand && state != S::Guard)
			Stand();
	}
	
	int res = animation.Update(time);
	if (res == -1)
	{
		animation = SpriteAnimation(standAnim);
		sprite = animation.Render();
	}
	else if (res == 1)
		sprite = animation.Render();
}

void Character::Turn(int direction) //LeftT, RightT, UpT, DownT
{
	dir = direction % 4; //change to Left Right Up Down
	vel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//state = S::Stand;
	state = direction;
	nextaction = S::Stand;
	locked = 0.0f;
	queue = 0.0f;
	cast = 0.0f;
}

void Character::Move(int direction, bool roll)
{
	nextaction = S::Stand;
	queue = 0.0f;
	cast = 0.0f;
	xstate = 0;

	int d = direction % 4;
	if ((d == S::Left && (cTile.x - 1 < 0 || !world->CanMoveTo(cTile.x - 1, cTile.y))) ||
		(d == S::Right && (cTile.x + 1 >= world->gridsize.x || !world->CanMoveTo(cTile.x + 1, cTile.y))) ||
		(d == S::Up && (cTile.y - 1 < 0 || !world->CanMoveTo(cTile.x, cTile.y - 1))) ||
		(d == S::Down && (cTile.y + 1 >= world->gridsize.y || !world->CanMoveTo(cTile.x, cTile.y + 1))))
	{
		state = S::Stand;
		locked = 0.0f;
		return;
	}

	float spd;
	if (!roll)
		spd = speed;
	else
		spd = rollspeed;

	locked = C::METER / spd;

	if (d == S::Left)
	{
		nTile.x = cTile.x - 1;
		vel.x = -spd;
	}
	else if (d == S::Right)
	{
		nTile.x = cTile.x + 1;
		vel.x = spd;
	}
	else if (d == S::Up)
	{
		nTile.y = cTile.y - 1;
		vel.y = -spd;
	}
	else if (d == S::Down)
	{
		nTile.y = cTile.y + 1;
		vel.y = spd;
	}
	npos.x = nTile.x * C::METER + C::HALF;
	npos.y = nTile.y * C::METER + C::HALF;

	world->SetTBOpen(cTile.x, cTile.y, true);
	world->SetTBOccupied(nTile.x, nTile.y, true);

	state = direction;
	tilelock = false;
}

void Character::Stand()
{
	state = S::Stand;
	nextaction = S::Stand;
	vel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	queue = 0.0f;
	cast = 0.0f;
	locked = 0.0f;
	xstate = 0;
}

void Character::HandleInput(int action, bool press)
{
	if (press)
	{
		if (action <= S::Down)
		{
			if (roll)
				nextaction = action + 4; //change to R
			else if (dir == action)
				nextaction = action;
			else
				nextaction = action + 8; //change to T
		}
		else if (S::Skill1 <= action && action <= S::Stand)
			nextaction = action;
		else if (action == S::Roll)
		{
			if (nextaction <= S::Down)
				nextaction += 4; //change to R

			roll = true;
		}
		keylock = true;
		queue = C::QUEUE_TIME;
		presstime = 0.0f;
	}
	else
	{
		if (action == S::Roll)
		{
			roll = false;
			keylock = false;
		}
		else if (action == S::Guard)
		{
			if (state == S::Guard)
			{
				state = S::Stand;
				nextaction = S::Stand;
			}
			guard = false;
			keylock = false;
		}
		else if (nextaction <= S::DownT && action == nextaction % 4)
			keylock = false;
		else if (S::Skill1 <= action && action <= S::Stand && action == nextaction)
			keylock = false;
	}
}