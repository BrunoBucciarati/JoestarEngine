#pragma once
#include "../Core/Object.h"
#include "../Container/Ptr.h"
#include "../Math/Rect.h"
#include "Viewport.h"
namespace Joestar
{
	class Camera;
	class Scene;
	class Graphics;
	class SwapChain;
	class View : public Object
	{
		REGISTER_OBJECT(View, Object);
	public:
		explicit View(EngineContext* ctx);
		bool Render();
		void Update(float);
		void SetRect(Rect& r)
		{
			mViewport.rect = r;
			mViewport.scissor = r;
		}
		void SetSwapChain(SwapChain* ptr)
		{
			mSwapChain = ptr;
		}
	private:
		UniquePtr<Camera> mCamera;
		UniquePtr<Scene> mScene;
		WeakPtr<Graphics> mGraphics;
		SwapChain* mSwapChain;
		Viewport mViewport;
	};
}