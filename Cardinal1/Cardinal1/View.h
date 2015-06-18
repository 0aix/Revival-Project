#ifndef VIEW_H
#define VIEW_H

#include "Main.h"
#include "Graphics.h"
#include "Misc.h"

class View
{
public:
	View()
	{
		d3d = Graphics::d3d;
		d3ddev = Graphics::d3ddev;
		sprite = Graphics::sprite;
		pInput = Cardinal::pInput;
	}
	virtual void Release() = 0; //must delete itself (?)
	virtual bool Update() = 0;
	virtual void HandleInput() = 0;
	virtual void Render() = 0;

protected:
	IDirect3D9* d3d;
	IDirect3DDevice9* d3ddev;
	ID3DXSprite* sprite;
	Input* pInput;
	D3DXVECTOR2 position;
	D3DXVECTOR2 size;
	bool bActive;
	bool bVisible;
};

struct ViewList
{
	View* pView;
	ViewList* pNext;

	~ViewList()
	{
		delete pView;
	}
};

class ViewManager
{
public:
	ViewManager()
	{
		pViewBase = new ViewList;
	}
	void Release(); //must delete itself and all the views
	void Update();
	void HandleInput();
	void Render();

	void AddView(View* view);

private:
	ViewList* pViewBase;
};

#endif