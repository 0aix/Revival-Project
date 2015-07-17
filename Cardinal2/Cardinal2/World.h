#pragma once
#include "Character.h"

class IWorld
{
public:
	IWorld(ICharacter* character, double w, double h, int cap)
	{
		players = new ICharacter*[cap];
		players[0] = character;
		width = w;
		height = h;
		capacity = cap;
	}
	~IWorld() { delete[] players; }
	virtual void Update() = 0;
	
	ICharacter** players;
	double width;
	double height;
	int capacity;
};

class SquareWorld : public IWorld
{
public:
	SquareWorld(ICharacter* character, double length, int cap);
	~SquareWorld();
	void Update();
};