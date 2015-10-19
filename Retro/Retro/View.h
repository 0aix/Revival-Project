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