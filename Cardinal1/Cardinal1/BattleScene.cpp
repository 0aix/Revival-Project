#include "Subscene.h"
#include "World.h"

BattleScene::BattleScene() : Subscene()
{
	world = new World(16, 16, 1);
	player = new Warrior(world, 7, 7, "Fang0r");
	world->players[0] = player;

	pScenes = new Subscene*[2];
	pScenes[0] = new WorldScene(player, world);
	pScenes[1] = new BattleUIScene();
}

void BattleScene::Release()
{
	Jotunheimr::UnloadResource(TO::PACK, TO::TILE_PACK);
	Jotunheimr::UnloadResource(TO::PACK, TO::WARRIOR_PACK);
	Jotunheimr::UnmapResource(TO::PACK, TO::BGM_PACK);
	delete this;
}

void BattleScene::Update()
{
	pScenes[0]->Update();
	pScenes[1]->Update();
}

void BattleScene::HandleInput()
{
	pScenes[0]->HandleInput();
	pScenes[1]->HandleInput();
}

void BattleScene::Render()
{
	//Subscenes do not have overhead
	pScenes[0]->Render();
	//pScenes[1]->Render();
}