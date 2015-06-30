#include "Scene.h"

LoadScene::LoadScene() : Scene()
{
	Jotunheimr::LoadResource(TO::PACK, TO::TILE_PACK, NULL);
	Jotunheimr::LoadResource(TO::PACK, TO::WARRIOR_PACK, NULL);
	Jotunheimr::MapResource(TO::PACK, TO::BGM_PACK, NULL);
}

void LoadScene::Release() 
{
	delete this;
}

void LoadScene::Update()
{
	if (++tick >= 200)
	{
		tick = 0;
		if (!Jotunheimr::LoadResource(TO::PACK, TO::TILE_PACK, NULL))
			return;
		if (!Jotunheimr::LoadResource(TO::PACK, TO::WARRIOR_PACK, NULL))
			return;
		if (!Jotunheimr::MapResource(TO::PACK, TO::BGM_PACK, NULL))
			return;
		Cardinal::NextScene(new MainScene());
		Release();
	}
}

void LoadScene::HandleInput() {}

void LoadScene::Render()
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF00000000, 1.0f, 0);
	d3ddev->Present(NULL, NULL, NULL, NULL);
}