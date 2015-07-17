#pragma once

#include <d3dx9.h>
#include "Constants.h"
#include "Misc.h"

struct Sprite
{
	IDirect3DTexture9* texture;
	D3DXVECTOR3 center;
	double delay;
};

struct SpriteAnim
{
	Sprite* sprites;
	int count;
	bool loop;

	SpriteAnim(int c, bool b)
	{
		sprites = new Sprite[c];
		count = c;
		loop = b;
	}
};

struct SpriteAnimation
{
	Sprite* sprites;
	double acc;
	int index;
	int count;
	bool loop;

	SpriteAnimation()
	{
		acc = 0.0f;
		index = 0;
	}

	SpriteAnimation(SpriteAnim* anim)
	{
		//assume sprites is filled with complete sprites
		sprites = anim->sprites;
		count = anim->count;
		loop = anim->loop;
		acc = 0.0f;
		index = 0;
	}

	//whether sprite changed
	//-1 done, 0 no change, 1 change
	int Update()
	{
		//could make a ready check here but.. naahhh
		acc += C::TICK;
		double delay = sprites[index].delay;
		if (acc >= delay)
		{
			acc -= delay;
			index++;
			if (index == count)
			{
				if (loop)
					index = 0;
				else
					return -1;
			}
			return 1;
		}
		return 0;
	}

	//well.. tech not needed but less referencing, should be faster
	Sprite* Render() { return &sprites[index]; }
};

struct Effect
{
	SpriteAnimation animation;
	Sprite* sprite;
	D3DXVECTOR3 pos;
	double radian;
};

typedef LList<Effect> EffectList;