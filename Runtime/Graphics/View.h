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
	class View : public Object
	{
		REGISTER_OBJECT(View, Object);
	public:
		explicit View(EngineContext* ctx);
		void Render();
		void Update(float);
		void SetRect(Rect& r)
		{
			mViewport.rect = r;
			mViewport.scissor = r;
		}
	private:
		UniquePtr<Camera> mCamera;
		UniquePtr<Scene> mScene;
		WeakPtr<Graphics> mGraphics;
		Viewport mViewport;
	};
}