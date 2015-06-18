#include "Subscene.h"

UIScene::UIScene(ViewManager* vm) : Subscene()
{
	VM = vm;
}

void UIScene::Release()
{
	delete this;
}

void UIScene::Update()
{

}

void UIScene::HandleInput()
{

}

void UIScene::Render()
{

}