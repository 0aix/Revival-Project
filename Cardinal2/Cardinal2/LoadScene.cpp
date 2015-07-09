#include "stdafx.h"
#include "View.h"

LoadScene::LoadScene() : IScene()
{
	IDirect3DTexture9* texture;
	d3ddev->CreateTexture(256, 256, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
	IDirect3DSurface9* backTarget;
	d3ddev->GetRenderTarget(0, &backTarget);
	IDirect3DSurface9* surfTarget;
	texture->GetSurfaceLevel(0, &surfTarget);
	d3ddev->SetRenderTarget(0, surfTarget);
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	d3ddev->BeginScene();
	d3ddev->SetFVF(VERTEX_2D_DIF::DIF2DFVF);
	VERTEX_2D_DIF buffer[256];
	float step = 2 * D3DX_PI / 256.0f;
	float radian = 0;
	float radius = 100.0f;
	for (int n = 0; n < 256; n++)
	{
		buffer[n].x = 128.0f + radius * cos(radian);
		buffer[n].y = 128.0f + radius * sin(radian);
		buffer[n].z = 0.0f;
		buffer[n].rhw = 1.0f;
		buffer[n].color = 0xFF00FFFF;
		radian += step;
	}
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 254, buffer, sizeof(VERTEX_2D_DIF));
	d3ddev->EndScene();
	d3ddev->SetRenderTarget(0, backTarget);
	Cardinal::SetState(0, texture);
}

void LoadScene::Release() { delete this; }
void LoadScene::Update() 
{
	Cardinal::NextScene(new MainScene());
	Release();
};
void LoadScene::HandleInput() {};
void LoadScene::Render() {};