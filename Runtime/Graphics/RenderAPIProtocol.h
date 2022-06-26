#pragma once
#include "../Platform/Platform.h"
#include "GPUResource.h"
#include "GPUCreateInfos.h"
#include "../Container/Vector.h"
#include "../Container/Ptr.h"
#include "../Graphics/CommandBuffer.h"
namespace Joestar
{
	class Window;
	class SwapChain;
	//Èí¼þÃèÊö·û
	class SoftwareDescriptorSets
	{
	public:
		GPUResourceHandle layoutHandle;
		GPUDescriptorSetsUpdateInfo updateInfo;
	};

	class RenderAPIProtocol
	{
	public:
		//Protocol Interfaces
		virtual void CreateDevice() = 0;
		virtual void CreateSwapChain(GPUSwapChainCreateInfo& createInfo, U32 num = 1);
		virtual void CreateSwapChain() = 0;
		virtual void CreateCommandPool(GPUResourceHandle handle, GPUQueue queue) = 0;
		virtual void CreateCommandBuffers(GPUResourceHandle handle, GPUCommandBufferCreateInfo& createInfo) = 0;
		virtual void CreateSyncObjects(U32 num = 1) = 0;
		virtual void CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo) = 0;
		virtual void CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo) = 0;
		virtual void CreateSampler(GPUResourceHandle handle, GPUSamplerCreateInfo& createInfo) = 0;
		virtual void CreateTexture(GPUResourceHandle handle, GPUTextureCreateInfo& createInfo) = 0;
		virtual void CreateIndexBuffer(GPUResourceHandle handle, GPUIndexBufferCreateInfo& createInfo) = 0;
		virtual void CreateVertexBuffer(GPUResourceHandle handle, GPUVertexBufferCreateInfo& createInfo) = 0;
		virtual void CreateUniformBuffer(GPUResourceHandle handle, GPUUniformBufferCreateInfo& createInfo) = 0;
		virtual void CreateRenderPass(GPUResourceHandle handle, GPURenderPassCreateInfo& createInfo) = 0;
		virtual void CreateDescriptorPool(U32 num = 1) = 0;
		virtual void CreateShader(GPUResourceHandle handle, GPUShaderCreateInfo& createInfo) = 0;
		virtual void SetUniformBuffer(GPUResourceHandle handle, U8* data, U32 size) = 0;
		virtual void CreateGraphicsPipelineState(GPUResourceHandle handle, GPUGraphicsPipelineStateCreateInfo& createInfo) = 0;
		virtual void CreateComputePipelineState(GPUResourceHandle handle, GPUComputePipelineStateCreateInfo& createInfo) = 0;
		virtual void QueueSubmit(GPUResourceHandle handle) = 0;
		virtual void Present() = 0;
		//Maybe Software Functions
		virtual void CreatePipelineLayout(GPUResourceHandle handle, GPUPipelineLayoutCreateInfo& createInfo);
		virtual void CreateDescriptorSetLayout(GPUResourceHandle handle, PODVector<GPUDescriptorSetLayoutBinding>& bindings);
		virtual void CreateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsCreateInfo& createInfo);
		virtual void UpdateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsUpdateInfo& updateInfo);
		///Uniform Functions
		void CreateMemory(GPUResourceHandle handle, U32 size, U8* data);
		virtual void CreateColorBlendState(GPUResourceHandle handle, GPUColorBlendStateCreateInfo& createInfo);
		virtual void CreateDepthStencilState(GPUResourceHandle handle, GPUDepthStencilStateCreateInfo& createInfo);
		virtual void CreateRasterizationState(GPUResourceHandle handle, GPURasterizationStateCreateInfo& createInfo);
		virtual void CreateMultiSampleState(GPUResourceHandle handle, GPUMultiSampleStateCreateInfo& createInfo);
		virtual void CreateShaderProgram(GPUResourceHandle handle, GPUShaderProgramCreateInfo& createInfo);
		void QueueSubmitCommandBuffer(GPUResourceHandle handle, U32 size, U8* data, U32);
		void SubmitCommandBuffer(GPUResourceHandle handle, U32 size, U8* data, U32);

		/// Command Buffer Protocols
		void RecordCommand(CommandBufferCMD& cmd, CommandEncoder& encoder, GPUResourceHandle handle);
		virtual void CBBegin(GPUResourceHandle handle) = 0;
		virtual void CBEnd(GPUResourceHandle handle) = 0;
		virtual void CBBeginRenderPass(GPUResourceHandle handle, RenderPassBeginInfo&) = 0;
		virtual void CBEndRenderPass(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBBindGraphicsPipeline(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBBindComputePipeline(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBBindIndexBuffer(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBBindVertexBuffer(GPUResourceHandle handle, GPUResourceHandle,U32=0) = 0;
		virtual void CBBindDescriptorSets(GPUResourceHandle handle, GPUResourceHandle, GPUResourceHandle, U32 = 0) = 0;
		virtual void CBPushConstants(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBDraw(GPUResourceHandle handle, U32 count) = 0;
		virtual void CBDrawIndexed(GPUResourceHandle handle, U32 count, U32 indexStart = 0, U32 vertStart = 0) = 0;
		virtual void CBCopyBuffer(GPUResourceHandle handle, CopyBufferType type, GPUResourceHandle) = 0;
		virtual void CBCopyBufferToImage(GPUResourceHandle handle, GPUResourceHandle, ImageLayout) = 0;
		virtual void CBTransitionImageLayout(GPUResourceHandle handle, GPUResourceHandle, ImageLayout, ImageLayout, U32) = 0;
		virtual void CBSubmit(GPUResourceHandle handle) = 0;
		virtual void CBSetViewport(GPUResourceHandle handle, const Viewport& vp) = 0;
		virtual void BeginFrame(U32 frameIndex)
		{
			SetFrame(frameIndex);
		}
		virtual void EndFrame(U32 frameIndex)
		{}

		void SetWindow(Window* w);
		void SetFrame(U32 frameIndex)
		{
			mFrameIndex = frameIndex;
		}
		GPUMultiSampleStateCreateInfo* GetMultiSampleState(GPUResourceHandle handle)
		{
			return mMultiSampleStates[handle];
		}
		GPUPipelineLayoutCreateInfo* GetPipelineLayout(GPUResourceHandle handle)
		{
			return mPipelineLayouts[handle];
		}
		GPUShaderProgramCreateInfo* GetShaderProgram(GPUResourceHandle handle)
		{
			return mShaderPrograms[handle];
		}
	protected:
		U32 maxMsaaSamples;
		U32 maxBindings{16};
		U32 maxUniformBuffers{ 1024 };
		U32 mFrameIndex{ 0 };
		Window* window;
		SwapChain* swapChain;
		GPUResourceHandle gCommandBufferHandle{0};
		GPUResourceHandle gPipelineStateHandle{0};
		Vector<GPUMemory> mMemories;
		Vector<GPUColorBlendStateCreateInfo*> mColorBlendStates;
		Vector<GPUDepthStencilStateCreateInfo*> mDepthStencilStates;
		Vector<GPURasterizationStateCreateInfo*> mRasterizationStates;
		Vector<GPUMultiSampleStateCreateInfo*> mMultiSampleStates;
		Vector<GPUShaderProgramCreateInfo*> mShaderPrograms;
		Vector<GPUPipelineLayoutCreateInfo*> mPipelineLayouts;
		Vector<PODVector<GPUDescriptorSetLayoutBinding>> mDescriptorSetLayouts;
		Vector<SoftwareDescriptorSets*> mDescriptorSets;
		//Vector<GPUGraphicsPipelineStateCreateInfo*> mGraphicsPipelineStates;
		//Vector<GPUComputePipelineStateCreateInfo*> mComputePipelineStates;
		bool bResized{ false };
		bool bSync{ false };
	};
}