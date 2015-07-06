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