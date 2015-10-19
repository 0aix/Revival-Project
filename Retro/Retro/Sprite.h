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
	Sprite* sprites = NULL;
	double acc = 0.0;
	int index = 0;
	int count;
	bool loop;
	double delay;
	double mult = 1.0;

	/*SpriteAnimation()
	{
		acc = 0.0f;
		index = 0;
	}*/

	SpriteAnimation(SpriteAnim* anim)
	{
		//assume sprites is filled with complete sprites
		sprites = anim->sprites;
		count = anim->count;
		loop = anim->loop;
		delay = sprites[0].delay;
	}

	//whether sprite changed
	bool Update()
	{
		//could make a ready check here but.. naahhh
		acc += mult * C::TICK;
		bool update = false;
		while (acc >= delay)
		{
			acc -= delay;
			index++;
			update = true;
			if (index == count && loop)
				index = 0;
			delay = sprites[index].delay;
		}
		return update;
	}

	//well.. tech not needed but less referencing, should be faster
	Sprite* Render() { return &sprites[index]; }
};

struct Effect
{
	SpriteAnimation animation;
	Sprite* sprite = NULL;
	D3DXVECTOR3 pos;
	double radian = 0.0; //**
};

typedef LList<Effect> EffectList;