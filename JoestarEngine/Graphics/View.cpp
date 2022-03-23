#include "View.h"
#include "../Base/Camera.h"
#include "../Scene/Scene.h"

namespace Joestar
{
	View::View(EngineContext* ctx) : Super(ctx)
	{
		mCamera = NEW_OBJECT(Camera);
		mScene = NEW_OBJECT(Scene);
	}

	View::~View()
	{}

	void View::Update(float dt)
	{
		if (mScene)
		{
			mScene->Update(dt);
		}
	}

	void View::Render()
	{
		if (mScene)
		{
			mScene->RenderScene(mCamera);
		}
	}
}