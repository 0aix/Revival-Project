#pragma once

#include "Character.h"

class World
{
public:
	World(ICharacter* character, double w, double h, int cap);
	~World();
	void Update();
	
	ICharacter** players;
	double width;
	double height;
	int capacity;

	BoxList* pBoxList = NULL;
	CircleList* pCircleList = NULL;
};