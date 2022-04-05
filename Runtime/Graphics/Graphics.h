#pragma once
#include "Mesh.h"
#include "../Thread/RenderThread.h"
#include "../Core/SubSystem.h"
#include "../Core/EngineContext.h"
#include "RenderCommand.h"
#include "../Math/Vector4.h"
#include "GraphicDefines.h"
#include "Material.h"
#include "../Math/Matrix4x4.h"
#include "Shader/Shader.h"
#include "ProgramCPU.h"
#include "CommandBuffer.h"
#include "PipelineState.h"
#include "GPUResource.h"
#include "FrameBuffer.h"
#include "UniformBuffer.h"
#include "Sampler.h"
namespace Joestar {
	class VertexBuffer;
	class IndexBuffer;
	class GFXCommandList;
	class CommandBuffer;
	class FrameBuffer;
	class SwapChain;
	class Shader;

	class Graphics : public SubSystem {
		REGISTER_SUBSYSTEM(Graphics)
	public:
		explicit Graphics(EngineContext* context);
		void Init();
		void Present();
		void Flush();
		void WaitForRender();
		void SubmitTransfer();
		void SubmitRender();
		void Clear();

		//NEW GRAPHICS
		CommandBuffer* GetMainCommandBuffer();
		CommandBuffer* GetTransferCommandBuffer();
		void CreateCommandBuffer(CommandBuffer* cb);

		FrameBuffer* GetBackBuffer();
		FrameBuffer* CreateFrameBuffer();
		void CreateDescriptorPool();
		void CreateCommandPool(CommandPool* pool);
		SwapChain* GetSwapChain();
		void CreateImage(GPUImage*, U32=1);
		void CreateImageView(GPUImageView*, U32=1);
		GPUMemory* CreateGPUMemory();
		void SetGPUMemory(GPUMemory*);
		GPUVertexBuffer* CreateGPUVertexBuffer(VertexBuffer*);
		void RemoveGPUVertexBuffer(GPUVertexBuffer* vb);
		GPUIndexBuffer* CreateGPUIndexBuffer(IndexBuffer*);
		void RemoveGPUIndexBuffer(GPUIndexBuffer* vb);
		void CreateUniformBuffer(UniformBuffer*);
		void CreateRenderPass(RenderPass*);
		void CreateGraphicsPipelineState(GraphicsPipelineState*);
		RenderPass* GetMainRenderPass();

		void CreateSwapChain();
		void CreateSyncObjects();
		void CreateShader(Shader* shader);
		void CreateShaderProgram(ShaderProgram* program);
		GFXCommandList* GetMainCmdList();

		RasterizationState* GetDefaultRasterizationState()
		{
			return mRasterizationStates[0];
		}
		MultiSampleState* GetDefaultMultiSampleState()
		{
			return mMultiSampleStates[0];
		}
		DepthStencilState* GetDefaultDepthStencilState()
		{
			return mDepthStencilStates[0];
		}
		ColorBlendState* GetDefaultColorBlendState()
		{
			return mColorBlendStates[0];
		}
		void CreateRasterizationState(RasterizationState*);
		void CreateMultiSampleState(MultiSampleState*);
		void CreateDepthStencilState(DepthStencilState*);
		void CreateColorBlendState(ColorBlendState*);

		void SetUniformBuffer(UniformBuffer*);
		void SetDescriptorSetLayout(DescriptorSetLayout* setLayout);
		void UpdateDescriptorSets(DescriptorSets* sets);
		void CreateDescriptorSets(DescriptorSets* sets);
		void CreatePipelineLayout(PipelineLayout*);
		void SetSampler(SharedPtr<Sampler>&);
		void QueueSubmit(CommandBuffer* cb);
		void Submit(CommandBuffer* cb);
		SharedPtr<DescriptorSets>& GetGlobalDescriptorSets()
		{
			return mDescriptorSets[0];
		}
	private:
		void CreatePerPassUniforms();
		void CreateMainRenderPass();
		void CreateDefaultStates();
		RenderThread* renderThread;
		Vector<GFXCommandBuffer*> cmdBuffers;
		Vector<GFXCommandBuffer*> computeCmdBuffers;
		GFXCommandBuffer* cmdBuffer;//current one
		GFXCommandBuffer* computeCmdBuffer;//current one
		Vector4f defaultClearColor;
		bool isCompute = false;
		U32 frameIdx = 0;

		Vector<SharedPtr<CommandPool>> mCommandPools;
		Vector<SharedPtr<CommandBuffer>> mCommandBuffers;
		Vector<SharedPtr<FrameBuffer>> mFrameBuffers;
		Vector<SharedPtr<GPUImage>> mImages;
		Vector<SharedPtr<GPUImageView>> mImageViews;
		Vector<SharedPtr<GPUMemory>> mGPUMemories;
		Vector<SharedPtr<GPUVertexBuffer>> mGPUVertexBuffers;
		Vector<SharedPtr<GPUIndexBuffer>> mGPUIndexBuffers;
		Vector<SharedPtr<UniformBuffer>> mUniformBuffers;
		Vector<SharedPtr<RenderPass>> mRenderPasses;
		Vector<SharedPtr<RasterizationState>> mRasterizationStates;
		Vector<SharedPtr<MultiSampleState>> mMultiSampleStates;
		Vector<SharedPtr<DepthStencilState>> mDepthStencilStates;
		Vector<SharedPtr<ColorBlendState>> mColorBlendStates;
		Vector<SharedPtr<PipelineLayout>> mPipelineLayouts;
		Vector<SharedPtr<GraphicsPipelineState>> mGraphicsPSOs;
		Vector<SharedPtr<ComputePipelineState>> mComputePSOs;
		Vector<SharedPtr<Shader>> mShaders;
		Vector<SharedPtr<ShaderProgram>> mShaderPrograms;
		HashMap<U32, SharedPtr<DescriptorSetLayout>> mDescriptorSetLayouts;
		Vector<SharedPtr<DescriptorSets>> mDescriptorSets;
		HashMap<U32, SharedPtr<Sampler>> mSamplerTable;
		SwapChain* mSwapChain;

		struct ThreadCommandList
		{
			GFXCommandList* cmdList[MAX_CMDLISTS_IN_FLIGHT];
		};
		Vector<ThreadCommandList> mThreadCommandLists;
		U32 msaaSamples{ 0 };
		bool bFloatingPointRT{ false };
		bool bStagingBuffer{ true };
	};
}