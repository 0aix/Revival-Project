#pragma once

#include "Subscene.h"

typedef LList<IView> ViewList;

class IView : public ISubscene
{
public:
	IView() : ISubscene() {}

protected:
	//Stuff...
};

//This might be utterly useless and can be replaced with just a ViewList or IView
class ViewManager
{
public:
	void Release(); //must delete itself and all views
	void Update();
	void HandleInput();
	void Render();

	void AddView(IView* view);

private:
	ViewList viewList;
};