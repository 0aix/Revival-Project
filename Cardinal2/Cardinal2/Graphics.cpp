#include "stdafx.h"
#include "Graphics.h"

namespace Graphics
{
	IDirect3D9* d3d = NULL;
	IDirect3DDevice9* d3ddev = NULL;
	ID3DXSprite* sprite = NULL;

	bool Initialize(HWND hwnd)
	{
		d3d = Direct3DCreate9(D3D_SDK_VERSION);

		if (!d3d)
			return false;

		D3DPRESENT_PARAMETERS d3dpp = { 0 };

		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = hwnd;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;

		if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev)))
			return false;

		if (FAILED(D3DXCreateSprite(d3ddev, &sprite)))
			return false;

		d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		/*gD3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		gD3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		gD3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);*/

		return true;
	}

	void Graphics::Uninitialize()
	{
		d3ddev->Release();
		d3d->Release();
	}
}

//METHOD 1 OF RENDERING ON TEXTURE
//d3ddev->CreateTexture(256, 256, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
//IDirect3DSurface9* backTarget;
//d3ddev->GetRenderTarget(0, &backTarget);
//IDirect3DSurface9* surfTarget;
//texture->GetSurfaceLevel(0, &surfTarget);
//d3ddev->SetRenderTarget(0, surfTarget);
//d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, 0xFFFF0000, 1.0f, 0);
//d3ddev->SetRenderTarget(0, backTarget);

//METHOD 2 OF RENDERING ON TEXTURE
//d3ddev->CreateTexture(256, 256, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, NULL);
//D3DLOCKED_RECT rect = { 0 };
//texture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
//DWORD* block = (DWORD*)rect.pBits;
//int pitch = rect.Pitch; //# of bytes in one row. in this case, 1024 since 4 bytes is the stride
//for (int row = 0; row < 256; row++)
//	for (int col = 0; col < 256; col++)
//		block[row * 256 + col] = 0xFFFF0000;
//texture->UnlockRect(0);

//NOTE: UPDATESURFACE AND UPDATETEXTURE CAN BE USED TO MISH-MASH DIFFERENT TEXTURES TOGETHER