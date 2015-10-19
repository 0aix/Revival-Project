#include "stdafx.h"
#include "View.h"

TestScene::TestScene() : IScene()
{
	d3ddev->SetFVF(VERTEX_2D_DIF::DIF2DFVF);

	IDirect3DTexture9* texture;
	d3ddev->CreateTexture(2, 2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
	IDirect3DSurface9* backTarget;
	d3ddev->GetRenderTarget(0, &backTarget);
	IDirect3DSurface9* surfTarget;
	texture->GetSurfaceLevel(0, &surfTarget);
	d3ddev->SetRenderTarget(0, surfTarget);
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, 0xFF000000, 1.0f, 0);
	surfTarget->Release();
	Cardinal::SetState(0, texture);
	d3ddev->SetRenderTarget(0, backTarget);
	backTarget->Release();

	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(25.0f, 37.5f);
	groundBody = world.CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(25.0f, 1.0f);
	groundBody->CreateFixture(&groundBox, 0.0f);

	// Define the dynamic body. We set its position and call the body factory.
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(25.0f, 10.0f);
	body = world.CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	body->CreateFixture(&fixtureDef);

	scaling = D3DXVECTOR2(16.0f, 16.0f);
	D3DXMatrixTransformation2D(&scalingmat, NULL, 0.0f, &scaling, NULL, 0.0f, NULL);

	square = (IDirect3DTexture9*)Cardinal::GetState(0);

	dTime = 0.0;
	clock.Start();
}

void TestScene::Release()
{
	char buff[256];
	sprintf(buff, "Render: %lf\nUpdate: %lf\nTick: %lf\n", rcount / time, ucount / time, tcount / time);
	MessageBoxA(NULL, buff, "", 0);
	square->Release();
	delete this;
}

void TestScene::Update()
{
	double d = clock.Time();
	dTime += d;
	time += d;
	ucount++;
	//dTime += clock.Time();
	while (dTime >= C::TICK)
	{
		tcount++;
		//if (!toggle)
		//	body->SetTransform(body->GetPosition(), body->GetAngle() + 0.08f);
		//body->ApplyAngularImpulse(0.10, true);
		angle += 0.08f;
		world.Step(C::TICK * 2.0, 4, 2);
		dTime -= C::TICK;
	}
	Render(); //
};

void TestScene::HandleInput()
{
	if (pInput->keycode == VK_SPACE)
	{
		toggle = !toggle;
		body->SetFixedRotation(toggle);
		if (toggle)
		{
			body->SetTransform(body->GetPosition(), 0.0f);
			body->SetGravityScale(0.0f);
		}
		else
			body->SetGravityScale(1.0f);
		body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	}

	if (toggle && pInput->state)
	{
		switch (pInput->keycode)
		{
		case VK_UP:
			body->SetLinearVelocity(b2Vec2(0.0f, -15.0f));
			break;
		case VK_LEFT:
			body->SetLinearVelocity(b2Vec2(-15.0f, 0.0f));
			break;
		case VK_DOWN:
			body->SetLinearVelocity(b2Vec2(0.0f, 15.0f));
			break;
		case VK_RIGHT:
			body->SetLinearVelocity(b2Vec2(15.0f, 0.0f));
		}
	}
	else if (pInput->state)
	{
		switch (pInput->keycode)
		{
		case VK_UP:
			body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, -15.0f));
			break;
		case VK_LEFT:
			body->SetLinearVelocity(b2Vec2(-15.0f, body->GetLinearVelocity().y));
			break;
		case VK_RIGHT:
			body->SetLinearVelocity(b2Vec2(15.0f, body->GetLinearVelocity().y));
		}
	}
};

void TestScene::Render()
{
	rcount++;
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);
	d3ddev->BeginScene();
	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	b2Vec2 position = body->GetPosition();
	//float angle = body->GetAngle();
	D3DXVECTOR2 center2d = D3DXVECTOR2(1.0f, 1.0f);
	D3DXMATRIX mat;
	D3DXVECTOR2 pos2d(position.x, position.y);
	Graphics::D3DX2DTransform(&mat, &center2d, angle, &pos2d, &scaling);
	sprite->SetTransform(&mat);
	sprite->Draw(square, NULL, NULL, NULL, 0xFFFFFFFF);

	sprite->End();
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
}