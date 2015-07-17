#include "stdafx.h"
#include "View.h"

LoadScene::LoadScene() : IScene()
{
	d3ddev->SetFVF(VERTEX_2D_DIF::DIF2DFVF);

	//Creating circle using method 1
	IDirect3DTexture9* texture;
	d3ddev->CreateTexture(256, 256, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
	IDirect3DSurface9* backTarget;
	d3ddev->GetRenderTarget(0, &backTarget);
	IDirect3DSurface9* surfTarget;
	texture->GetSurfaceLevel(0, &surfTarget);
	d3ddev->SetRenderTarget(0, surfTarget);
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	d3ddev->BeginScene();
	VERTEX_2D_DIF buffer[256];
	float step = 2 * D3DX_PI / 256.0f;
	float radian = 0.0f;
	float radius = 100.0f;
	for (int n = 0; n < 256; n++)
	{
		buffer[n].x = 128.0f + radius * cos(radian);
		buffer[n].y = 128.0f + radius * sin(radian);
		buffer[n].z = 0.0f;
		buffer[n].rhw = 1.0f;
		buffer[n].color = 0xFFFF1414;
		radian += step;
	}
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 254, buffer, sizeof(VERTEX_2D_DIF));
	d3ddev->EndScene();
	surfTarget->Release();
	Cardinal::SetState(0, texture);

	//Creating arrow using method 1
	d3ddev->CreateTexture(100, 75, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
	texture->GetSurfaceLevel(0, &surfTarget);
	d3ddev->SetRenderTarget(0, surfTarget);
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	d3ddev->BeginScene();
	step = 2 * D3DX_PI / 16.0f;
	radius = 3.5f;
	radian = 0.0f;
	for (int n = 0; n < 16; n++)
	{
		buffer[n].x = -12.5f + radius * cos(radian);
		buffer[n].y = 37.5f + radius * sin(radian);
		buffer[n].color = 0xBDFF400A;
		radian += step;
	}
	for (int c = 0; c < 3; c++)
	{
		for (int n = 0; n < 16; n++)
			buffer[n].x += 25.0f;
		d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 14, buffer, sizeof(VERTEX_2D_DIF));
	}
	buffer[0].x = 62.5f;
	buffer[0].y = 9.0f;
	buffer[1].x = 91.0f;
	buffer[1].y = 37.5f;
	buffer[2].x = 84.0f;
	buffer[2].y = 37.5f;
	buffer[3].x = 62.5f;
	buffer[3].y = 16.0f;
	buffer[4].x = 84.0f;
	buffer[4].y = 37.5f;
	buffer[5].x = 91.0f;
	buffer[5].y = 37.5f;
	buffer[6].x = 62.5f;
	buffer[6].y = 66.0f;
	buffer[7].x = 62.5f;
	buffer[7].y = 59.0f;
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, buffer, sizeof(VERTEX_2D_DIF));
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, buffer + 4, sizeof(VERTEX_2D_DIF));
	d3ddev->EndScene();
	surfTarget->Release();
	Cardinal::SetState(1, texture);

	//Creating tile map using method 1
	d3ddev->CreateTexture(2304, 1792, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
	texture->GetSurfaceLevel(0, &surfTarget);
	d3ddev->SetRenderTarget(0, surfTarget);
	//since this thing is larger than the screen, im gonna replace the depth stencil buffer
	IDirect3DTexture9* stencil;
	d3ddev->CreateTexture(2304, 1792, 1, D3DUSAGE_DEPTHSTENCIL, D3DFMT_D16, D3DPOOL_DEFAULT, &stencil, NULL);
	IDirect3DSurface9* backStencil;
	d3ddev->GetDepthStencilSurface(&backStencil);
	IDirect3DSurface9* surfStencil;
	stencil->GetSurfaceLevel(0, &surfStencil);
	d3ddev->SetDepthStencilSurface(surfStencil);
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, 0xFF000000, 1.0f, 0);
	d3ddev->BeginScene();
	int i = 0;
	for (int x = 0; x < 9; x++)
	{
		float px = x * 256.0f;
		for (int y = 0; y < 7; y++)
		{
			float py = y * 256.0f;
			buffer[i].x = px + 5.0f;
			buffer[i].y = py + 5.0f;
			buffer[i + 3].x = px + 5.0f;
			buffer[i + 3].y = py + 251.0f;
			buffer[i + 2].x = px + 251.0f;
			buffer[i + 2].y = py + 251.0f;
			buffer[i + 1].x = px + 251.0f;
			buffer[i + 1].y = py + 5.0f;
			i += 4;
		}
	}
	for (int n = 0; n < 252; n++)
		buffer[n].color = 0xFFFFFFFF;
	for (int n = 0; n < 63; n++)
		d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, buffer + n * 4, sizeof(VERTEX_2D_DIF));
	d3ddev->EndScene();
	d3ddev->SetDepthStencilSurface(backStencil);
	surfTarget->Release();
	surfStencil->Release();
	backStencil->Release();
	Cardinal::SetState(2, texture);

	d3ddev->SetRenderTarget(0, backTarget);
	backTarget->Release();
}

void LoadScene::Release() { delete this; }
void LoadScene::Update() 
{
	Cardinal::NextScene(new MainScene());
	Release();
};
void LoadScene::HandleInput() {};
void LoadScene::Render() {};