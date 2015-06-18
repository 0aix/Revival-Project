#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Main.h"
#include <d3dx9.h>

namespace Graphics
{
	extern IDirect3D9* d3d;
	extern IDirect3DDevice9* d3ddev;
	extern ID3DXSprite* sprite;

	bool Initialize(HWND hwnd);
	void Uninitialize();
}

#endif