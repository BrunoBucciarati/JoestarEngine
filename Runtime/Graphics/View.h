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
	class CommandBuffer;
	class HID;
	class GameObject;
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
		void RenderScene(CommandBuffer* cb);
		void RenderSkybox(CommandBuffer* cb);
		void CreatePassDescriptor();
		void SetUniformBuffer(PerPassUniforms, U8* data);
		void ForwardRender(CommandBuffer* cb);
		UniquePtr<Camera> mCamera;
		UniquePtr<Camera> mShadowCamera;
		UniquePtr<Scene> mScene;
		WeakPtr<Graphics> mGraphics;
		WeakPtr<HID> mHID;
		SwapChain* mSwapChain;
		Viewport mViewport;
		SharedPtr<DescriptorSets> mDescriptorSets;
		SharedPtr<DescriptorSetLayout> mDescriptorSetLayout;
		Vector<SharedPtr<UniformBuffer>> mUniformBuffers;
		SharedPtr<GameObject> mShadowCameraNode;
		SharedPtr<GameObject> mCameraNode;
	};
}