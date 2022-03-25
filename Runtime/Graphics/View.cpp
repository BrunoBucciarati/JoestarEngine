#include "View.h"
#include "../Base/Camera.h"
#include "../Scene/Scene.h"

namespace Joestar
{
	View::View(EngineContext* ctx) : Super(ctx)
	{
		mCamera = NEW_OBJECT(Camera);
		mScene = NEW_OBJECT(Scene);
		mGraphics = GetSubsystem<Graphics>();
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
		CommandBuffer* cb = mGraphics->GetMainCommandBuffer();
		if (mScene)
		{
			mScene->RenderScene(mCamera);
		}
	}
}