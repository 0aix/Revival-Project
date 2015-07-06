#pragma once

#include "Scene.h"

class ISubscene : public IScene
{
public:
	ISubscene() : IScene() {}

protected:
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	bool bActive = true;
	bool bVisible = true;
};