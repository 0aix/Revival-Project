#pragma once

#include <d3dx9.h>

struct VERTEX_2D_DIF
{
	float x, y, z, rhw;
	DWORD color;
	
	static const DWORD DIF2DFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
};

namespace Graphics
{
	extern IDirect3D9* d3d;
	extern IDirect3DDevice9* d3ddev;
	extern ID3DXSprite* sprite;

	bool Initialize(HWND hwnd);
	void Uninitialize();
}