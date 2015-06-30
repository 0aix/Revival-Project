#include "Subscene.h" //#include "Scene.h"

MainScene::MainScene() : Scene()
{
	pScenes = new Subscene*[2];
	pScenes[0] = new BattleScene();
	pScenes[1] = new UIScene(VM);
	Pack* pack;
	Jotunheimr::MapResource(TO::PACK, TO::BGM_PACK, (void**)&pack);
	pVoice = Audio::CreateVoice();
	for (int i = 0; i < 3; i++)
	{
		pSounds[i] = Audio::CreateSound((BUFFER*)(pack->pack[i]));
		pVoice->Queue(pSounds[i]);
	}
	pVoice->Loop();
	pVoice->Start();
}

void MainScene::Release()
{
	pScenes[0]->Release();
	pScenes[1]->Release();
	if (pVoice)
		pVoice->Destroy();
	for (int i = 0; i < 3; i++)
		delete pSounds[i];
	Jotunheimr::UnmapResource(TO::PACK, TO::BGM_PACK);
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
	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	pScenes[0]->Render();
	pScenes[1]->Render();

	sprite->End();
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
}