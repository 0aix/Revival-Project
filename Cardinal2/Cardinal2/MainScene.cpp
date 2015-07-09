#include "stdafx.h"
#include "View.h"

MainScene::MainScene() : IScene()
{
	pScenes = new ISubscene*;
	pScenes[0] = new WorldScene;
	circle = (IDirect3DTexture9*)Cardinal::GetState(0);
}

void MainScene::Release() 
{
	circle->Release();
	delete this;
}

void MainScene::Update() 
{
	pScenes[0]->Update();
}

void MainScene::HandleInput() 
{
	pScenes[0]->HandleInput();
}

void MainScene::Render() 
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF000000, 1.0f, 0);
	d3ddev->BeginScene();
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	
	pScenes[0]->Render();

	sprite->End();
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
}