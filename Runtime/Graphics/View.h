#pragma once
#include "../Core/Minimal.h"
#include "../Math/Rect.h"
#include "Viewport.h"
#include "GraphicDefines.h"
namespace Joestar
{
	class Camera;
	class Scene;
	class Graphics;
	class SwapChain;
	class DescriptorSets;
	class DescriptorSetLayout;
	class UniformBuffer;
	class HID;
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
		void CreatePassDescriptor();
		void SetUniformBuffer(PerPassUniforms, U8* data);
		UniquePtr<Camera> mCamera;
		UniquePtr<Scene> mScene;
		WeakPtr<Graphics> mGraphics;
		WeakPtr<HID> mHID;
		SwapChain* mSwapChain;
		Viewport mViewport;
		SharedPtr<DescriptorSets> mDescriptorSets;
		SharedPtr<DescriptorSetLayout> mDescriptorSetLayout;
		Vector<SharedPtr<UniformBuffer>> mUniformBuffers;
	};
}