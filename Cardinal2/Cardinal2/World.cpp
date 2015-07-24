#include "stdafx.h"
#include "World.h"
#include "Ball.h"

World::World(ICharacter* character, double w, double h, int cap)
{
	players = new ICharacter*[cap] { 0 };
	for (int i = 0; i < cap; i++)
		players[i] = NULL;
	players[0] = character;
	width = w;
	height = h;
	capacity = cap;

	Line L[4];
	L[0] = Line(0.0, 0.0, 0.0, h);
	L[1] = Line(0.0, h, w, h);
	L[2] = Line(w, h, w, 0.0);
	L[3] = Line(w, 0.0, 0.0, 0.0);
	pBoxBase = new BoxList;
	pBoxBase->pData = new Box(L, 4, 0.0, 0.0, 0.0, 1.0);
}

World::~World()
{
	delete[] players;
}

//Set Z for everything
void World::Update()
{
	//Update everything first
	for (int i = 0; i < capacity; i++)
	{
		ICharacter* player = players[i];
		if (player)
			player->Update();
		//set z

	}

	//Update map box objects (lol, i dont have any that move)
	//Update map circle objects (lol, i dont have any)

	//Check for collisions second
	for (int i = 0; i < capacity; i++)
	{
		ICharacter* player = players[i];
		if (player)
		{
			Collision coll;
			Circle& circle = player->circle;

			//player collision with map boxes
			BoxList* pBoxNode = pBoxBase;
			while (pBoxNode != NULL)
			{
				Box* pBox = pBoxNode->pData;
				if (Physics::CircleToBox(circle, *pBox, &coll))
					Physics::ResolveCollision(circle, *pBox, coll);
				
				//player's balls (only circles)
				BallList* pBallNode = player->pBallBase;
				while (pBallNode != NULL)
				{
					IBall* pBall = pBallNode->pData;
					Circle* pCircle = pBall->circle;
					if (pCircle != NULL && Physics::CircleToBox(*pCircle, *pBox, &coll))
						if (pBall->Hit(COLL::OBJ, NULL) == COLL::BOUNCE)
							Physics::ResolveCollision(*pCircle, *pBox, coll);
					pBallNode = pBallNode->pNext;
				}

				pBoxNode = pBoxNode->pNext;
			}

			//Player collision with map circles
			CircleList* pCircleNode = pCircleBase;
			while (pCircleNode != NULL)
			{
				Circle* temp = pCircleNode->pData;
				if (Physics::CircleToCircle(circle, *temp, &coll))
					Physics::ResolveCollision(circle, *temp, coll);

				//player's balls
				BallList* pBallNode = player->pBallBase;
				while (pBallNode != NULL)
				{
					IBall* pBall = pBallNode->pData;
					Circle* pCircle = pBall->circle;
					Box* pBox = pBall->box;
					//Note at least one of these hitboxes is null
					if ((pBox != NULL && Physics::CircleToBox(*temp, *pBox, &coll)) ||
						(pCircle != NULL && Physics::CircleToCircle(*temp, *pCircle, &coll)))
					{
						if (pBall->Hit(COLL::OBJ, NULL) == COLL::BOUNCE)
						{
							if (pBox != NULL)
								Physics::ResolveCollision(*temp, *pBox, coll);
							else if (pCircle != NULL)
								Physics::ResolveCollision(*temp, *pCircle, coll);
						}
					}
					pBallNode = pBallNode->pNext;
				}

				pCircleNode = pCircleNode->pNext;
			}

			//player-player collisions
			for (int n = i + 1; n < capacity; n++)
			{
				ICharacter* player2 = players[n];
				if (player2)
				{
					//player to player collision
					Circle& circle2 = player2->circle;
					if (Physics::CircleToCircle(circle, circle2, &coll))
						Physics::ResolveCollision(circle, circle2, coll);

					//player's balls to player collisions
					BallList* pBallNode = player->pBallBase;
					while (pBallNode != NULL)
					{
						IBall* pBall = pBallNode->pData;
						Circle* pCircle = pBall->circle;
						Box* pBox = pBall->box;
						COLL type;
						//Note at least one of these hitboxes is null
						if ((pBox != NULL && Physics::CircleToBox(circle2, *pBox, &coll)) ||
							(pCircle != NULL && Physics::CircleToCircle(circle2, *pCircle, &coll)))
						{
							if (pBall->Hit(COLL::HIT, player2) == COLL::BOUNCE)
							{
								if (pBox != NULL)
									Physics::ResolveCollision(circle2, *pBox, coll);
								else if (pCircle != NULL)
									Physics::ResolveCollision(circle2, *pCircle, coll);
							}
						}

						//to player's balls collisions
						BallList* pBallNode2 = player2->pBallBase;
						while (pBallNode2 != NULL)
						{
							IBall* pBall2 = pBallNode2->pData;
							Circle* pCircle2 = pBall2->circle;
							Box* pBox2 = pBall2->box;
							
							if (pBox != NULL)
							{
								if (pBox2 != NULL && Physics::BoxToBox(*pBox, *pBox2))
								{
									pBall->Hit(COLL::BALL, NULL);
									pBall2->Hit(COLL::BALL, NULL);
								}
								else if (pCircle2 != NULL && Physics::CircleToBox(*pCircle2, *pBox, &coll))
									if (pBall->Hit(COLL::BALL, NULL) == COLL::BOUNCE && pBall2->Hit(COLL::BALL, NULL) == COLL::BOUNCE)
										Physics::ResolveCollision(*pCircle2, *pBox, coll);
							}
							else if (pCircle != NULL)
							{
								if (pBox2 != NULL && Physics::CircleToBox(*pCircle, *pBox2, &coll))
								{
									if (pBall->Hit(COLL::BALL, NULL) == COLL::BOUNCE && pBall2->Hit(COLL::BALL, NULL) == COLL::BOUNCE)
										Physics::ResolveCollision(*pCircle, *pBox2, coll);
								}
								else if (pCircle2 != NULL && Physics::CircleToCircle(*pCircle, *pCircle2, &coll))
									if (pBall->Hit(COLL::BALL, NULL) == COLL::BOUNCE && pBall2->Hit(COLL::BALL, NULL) == COLL::BOUNCE)
										Physics::ResolveCollision(*pCircle, *pCircle2, coll);
							}

							if ((pBox != NULL && Physics::CircleToBox(circle2, *pBox, &coll)) ||
								(pCircle != NULL && Physics::CircleToCircle(circle2, *pCircle, &coll)))
							{
								type = pBall->Hit(COLL::HIT, player2);
								if (type == COLL::BOUNCE)
								{
									if (pBox != NULL)
										Physics::ResolveCollision(circle2, *pBox, coll);
									else if (pCircle != NULL)
										Physics::ResolveCollision(circle2, *pCircle, coll);
								}
							}
							pBallNode2 = pBallNode2->pNext;
						}
						pBallNode = pBallNode->pNext;
					}
				}
			}
		}
	}
}