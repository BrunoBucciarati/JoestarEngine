#pragma once
#include "../Core/Minimal.h"
#include "../Math/Rect.h"
#include "Viewport.h"
#include "GraphicDefines.h"
#include "Batch.h"
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
	class RenderPass;
	class FrameBuffer;
	class Texture2D;
	enum class Pass
	{
		Shadow = 0,
		Scene,
		Count
	};
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
		Graphics* GetGraphics()
		{
			return mGraphics;
		}
	private:
		void CreatePassDescriptor();
		void SetUniformBuffer(PerPassUniforms, U8* data, Pass pass = Pass::Scene);
		void ForwardRender(CommandBuffer* cb);
		void CollectBatches();
		void CollectShadowBatches();
		void InitShadowPass();
		UniquePtr<Camera> mCamera;
		UniquePtr<Camera> mShadowCamera;
		UniquePtr<Scene> mScene;
		WeakPtr<Graphics> mGraphics;
		WeakPtr<HID> mHID;
		SwapChain* mSwapChain;
		Viewport mViewport;
		Vector<SharedPtr<DescriptorSets>> mAllDescriptorSets;
		SharedPtr<DescriptorSetLayout> mDescriptorSetLayout;
		Vector<Vector<SharedPtr<UniformBuffer>>> mAllUniformBuffers;
		SharedPtr<GameObject> mShadowCameraNode;
		SharedPtr<GameObject> mCameraNode;
		Vector<Batch> mBatches;
		Vector<Batch> mShadowBatches;
		SharedPtr<MaterialInstance> mShadowMaterial;
		SharedPtr<RenderPass> mShadowPass;
		SharedPtr<FrameBuffer> mShadowFrameBuffer;
		SharedPtr<Texture2D> mShadowMap;
	};
}