#include "View.h"
#include "../Base/Camera.h"
#include "../Scene/Scene.h"
#include "GraphicDefines.h"

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
		//mGraphics->SetUniformBuffer(PerPassUniforms::VIEW_MATRIX, (float*)mCamera->GetViewMatrix());
		//mGraphics->SetUniformBuffer(PerPassUniforms::PROJECTION_MATRIX, (float*)mCamera->GetProjectionMatrix());
		CommandBuffer* cb = mGraphics->GetMainCommandBuffer();
		cb->Begin();
		cb->SetViewport(&mViewport);
		cb->BeginRenderPass(mGraphics->GetMainRenderPass(), mGraphics->GetBackBuffer());
		if (mScene)
		{
			mScene->RenderScene(cb);
		}
		cb->EndRenderPass(mGraphics->GetMainRenderPass());
		cb->End();
		mGraphics->QueueSubmit(cb);
	}
}