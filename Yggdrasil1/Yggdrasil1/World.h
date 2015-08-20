#pragma once

#include "Character.h"

class World
{
public:
	World(double w, double h, int cap);
	~World();
	void Update();

	ICharacter** players;
	double width;
	double height;
	int capacity;

	BoxList boxList;
	CircleList circleList;
};