#pragma once
#include "../Core/Object.h"
#include "../Container/Ptr.h"
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
	private:
		UniquePtr<Camera> mCamera;
		UniquePtr<Scene> mScene;
		WeakPtr<Graphics> mGraphics;
	};
}