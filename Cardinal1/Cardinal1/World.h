#ifndef WORLD_H
#define WORLD_H

#include "Main.h"
#include <d3dx9.h>
#include "Sprite.h"

class World;
class Character;
struct Ball;
struct Effect;
struct EffectList;

typedef void(*Call)(Character* target, Ball* ball); //ball is this
typedef void(*Coll)(Ball* other, Ball* ball); //ball is this
typedef void(*Param)(Ball* ball); //ball is this

struct Tile
{
	byte type; //Like what sprite to use. might have a TileAnimator to change the value to animate
	byte flags; // 0 | 0 | 0 | ObjWall | Wall | To be Occupied | To be Unoccupied | Occupied
};

//enum; will cover everything
enum S
{
	Left, Right, Up, Down,
	LeftR, RightR, UpR, DownR,
	LeftT, RightT, UpT, DownT,
	Skill1, Skill2, Skill3, Skill4,
	Attack, Guard, Stand, Hit, Roll
};

//maybe packets -> struct with info?
class Character
{
public:
	Character(World* map)
	{
		world = map;
	}
	virtual void Attack() = 0;
	virtual void Guard() = 0;
	virtual void Skill1() = 0;
	virtual void Skill2() = 0;
	virtual void Skill3() = 0;
	virtual void Skill4() = 0;
	virtual void UpdateA(float time) = 0;
	virtual bool Hit(int direction, int type) = 0;
	void Update(float time);
	void Turn(int direction);
	void Move(int direction, bool roll);
	void Stand();
	void HandleInput(int action, bool press);

	POINT cTile; //current
	POINT nTile; //next
	D3DXVECTOR3 pos;
	D3DXVECTOR3 npos;
	D3DXVECTOR3 vel;
	int state;
	int dir;

	Sprite* sprite;
	SpriteAnimation animation;
	SpriteAnim* standAnim;

protected:
	World* world;
	float maxhp, maxmp, maxsp;
	float hp, mp, sp;
	float hpr, mpr, spr;
	float cooldown[4];
	float speed;
	float rollspeed;
	float cast;
	float locked;
	int nextaction;
	float queue;
	float autochain;
	int lastauto;
	float presstime;
	int xstate;
	bool tilelock;
	bool roll;
	bool keylock;
	bool guard;

	char* name;
	int kills;
	int deaths;
	
};

class Warrior : public Character
{
public:
	Warrior(World* map, int tx, int ty, char* cname);
	void Attack();
	void Guard();
	void Skill1();
	void Skill2();
	void Skill3();
	void Skill4();
	void UpdateA(float time);
	bool Hit(int direction, int type);

private:

};

struct Ball
{
	Ball(Character* c, World* map, D3DXVECTOR3 off, POINT t, float r, float d, int hp, bool col, bool p, Param param, Call hit, Coll coll);
	bool Update(float time);
	//void SetEffect(     );
	Character* player;
	World* world;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 offset;
	D3DXVECTOR3 fpos;
	POINT tile;
	Param parfunc;
	Call hitfunc;
	Coll colfunc;
	float radius;
	bool collision; //w/ walls
	bool pierce;
	float duration;
	float acc;
	
	int hits;

};

struct BallList
{
	Ball* pBall;
	BallList* pNext;

	~BallList() { delete pBall; }
};

class World
{
public:
	World(int width, int height, int cap);
	~World();
	void Update(float time);
	bool Occupied(int x, int y);
	bool TBOpen(int x, int y);
	bool TBOccupied(int x, int y);
	bool Wall(int x, int y);
	bool ObjWall(int x, int y);
	bool CanMoveTo(int x, int y);
	void SetOccupied(int x, int y, bool b);
	void SetTBOpen(int x, int y, bool b);
	void SetTBOccupied(int x, int y, bool b);
	void SetWall(int x, int y, bool b);
	void SetObjWall(int x, int y, bool b);
	void AddBall(Ball* ball);
	void AddEffect(Effect* effect);
	Character* At(int x, int y);

	POINT gridsize;
	D3DXVECTOR3 size;
	Tile** grid;
	Character** players;
	BallList* pBallBase; //null start node
	EffectList* pEffectBase; //null start node
	int capacity;

private:

};

#endif