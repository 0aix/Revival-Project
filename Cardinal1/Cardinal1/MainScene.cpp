#include "Scene.h"
#include "Subscene.h"

MainScene::MainScene() : Scene()
{
	pScenes = new Subscene*[2];
	pScenes[0] = new BattleScene();
	pScenes[1] = new UIScene(VM);
}

void MainScene::Release()
{

	delete this;
}

void MainScene::Update()
{
	pScenes[0]->Update(); 
	pScenes[1]->Update();
}

void MainScene::HandleInput()
{
	pScenes[1]->HandleInput();
	pScenes[0]->HandleInput();
}

void MainScene::Render()
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);
	d3ddev->BeginScene();

	pScenes[0]->Render();
	pScenes[1]->Render();

	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
}