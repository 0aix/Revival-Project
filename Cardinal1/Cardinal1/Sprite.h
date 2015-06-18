#ifndef SPRITE_H
#define SPRITE_H

#include <d3dx9.h>

struct Sprite
{
	IDirect3DTexture9* texture;
	D3DXVECTOR3 center;
	float delay;
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
	float acc;
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
	//-1 no more, 0 no change, 1 change
	int Update(float time)
	{
		//could make a ready check here but.. naahhh
		acc += time;
		if (acc >= sprites[index].delay)
		{
			acc -= sprites[index].delay;
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
	Sprite* Render()
	{
		return &sprites[index];
	}
};

struct Effect
{
	SpriteAnimation animation;
	Sprite* sprite;
	D3DXVECTOR3 pos;
};

struct EffectList
{
	Effect* pEffect;
	EffectList* pNext;

	~EffectList() { delete pEffect; }
};

#endif