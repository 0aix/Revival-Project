#pragma once

#include <d3dx9.h>

struct VERTEX_2D_DIF
{
	float x, y, z = 0.0f, rhw = 1.0f;
	DWORD color = 0xFFFFFFFF;

	static const DWORD DIF2DFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
};

namespace Graphics
{
	extern IDirect3D9* d3d;
	extern IDirect3DDevice9* d3ddev;
	extern ID3DXSprite* sprite;

	bool Initialize(HWND hwnd);
	void Uninitialize();

	//Helper functions
	D3DXMATRIX* D3DX2DTransform(D3DXMATRIX* pOut, D3DXVECTOR2* center, float radian, D3DXVECTOR2* translation, D3DXVECTOR2* scaling);
}