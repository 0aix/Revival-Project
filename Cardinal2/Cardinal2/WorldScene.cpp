#include "stdafx.h"
#include "Subscene.h"

WorldScene::WorldScene() : ISubscene()
{
	clock.Start();
}

void WorldScene::Release()
{
	delete this;
}

void WorldScene::Update() {}
void WorldScene::HandleInput() {}
void WorldScene::Render() {}