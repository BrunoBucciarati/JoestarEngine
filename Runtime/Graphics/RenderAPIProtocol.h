#pragma once
#include "../Platform/Platform.h"
#include "GPUResource.h"
#include "GPUCreateInfos.h"
#include "../Container/Vector.h"
#include "../Graphics/CommandBuffer.h"
namespace Joestar
{
	class Window;

	class RenderAPIProtocol
	{
	public:
		//Protocol Interfaces
		virtual void CreateDevice() = 0;
		virtual void CreateSwapChain(GPUSwapChainCreateInfo& createInfo, U32 num = 1) = 0;
		virtual void CreateMainCommandBuffers(U32 num = 1) = 0;
		virtual void CreateCommandBuffers(GPUResourceHandle handle, GPUCommandBufferCreateInfo& createInfo, U32 num = 1) = 0;
		virtual void CreateSyncObjects(U32 num = 1) = 0;
		virtual void CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo) = 0;
		virtual void CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo) = 0;
		virtual void CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo) = 0;
		virtual void CreateIndexBuffer(GPUResourceHandle handle, GPUIndexBufferCreateInfo& createInfo) = 0;
		virtual void CreateVertexBuffer(GPUResourceHandle handle, GPUVertexBufferCreateInfo& createInfo) = 0;
		virtual void CreateUniformBuffer(GPUResourceHandle handle, GPUUniformBufferCreateInfo& createInfo) = 0;
		virtual void CreateRenderPass(GPUResourceHandle handle, GPURenderPassCreateInfo& createInfo) = 0;
		virtual void CreateDescriptorPool(U32 num = 1) = 0;
		virtual void CreateGraphicsPipelineState(GPUResourceHandle handle, GPUGraphicsPipelineStateCreateInfo& createInfo) = 0;
		virtual void CreateComputePipelineState(GPUResourceHandle handle, GPUComputePipelineStateCreateInfo& createInfo) = 0;
		virtual void CreateShader(GPUResourceHandle handle, GPUShaderCreateInfo& createInfo) = 0;
		virtual void SetUniformBuffer(GPUResourceHandle handle, U8* data, U32 size) = 0;
		virtual void CreatePipelineLayout(GPUResourceHandle handle, GPUPipelineLayoutCreateInfo& createInfo) = 0;
		virtual void CreateDescriptorSetLayout(GPUResourceHandle handle, PODVector<DescriptorSetLayoutBinding>& bindings) {};
		virtual void CreateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsCreateInfo& createInfo) = 0;
		virtual void UpdateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsUpdateInfo& updateInfo) = 0;
		///Uniform Functions
		void CreateMemory(GPUResourceHandle handle, U32 size, U8* data);
		void CreateColorBlendState(GPUResourceHandle handle, GPUColorBlendStateCreateInfo& createInfo);
		void CreateDepthStencilState(GPUResourceHandle handle, GPUDepthStencilStateCreateInfo& createInfo);
		void CreateRasterizationState(GPUResourceHandle handle, GPURasterizationStateCreateInfo& createInfo);
		void CreateMultiSampleState(GPUResourceHandle handle, GPUMultiSampleStateCreateInfo& createInfo);
		void CreateShaderProgram(GPUResourceHandle handle, GPUShaderProgramCreateInfo& createInfo);
		void QueueSubmit(GPUResourceHandle handle, U32 size, U8* data);

		/// Command Buffer Protocols
		void RecordCommand(CommandBufferCMD& cmd, CommandEncoder& encoder, GPUResourceHandle handle);
		virtual void CBBegin(GPUResourceHandle handle) = 0;
		virtual void CBEnd(GPUResourceHandle handle) = 0;
		virtual void CBBeginRenderPass(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBEndRenderPass(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBBindGraphicsPipeline(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBBindComputePipeline(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBBindIndexBuffer(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBBindVertexBuffer(GPUResourceHandle handle, GPUResourceHandle,U32=0) = 0;
		virtual void CBBindDescriptorSets(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBPushConstants(GPUResourceHandle handle, GPUResourceHandle) = 0;
		virtual void CBDraw(GPUResourceHandle handle, U32 count) = 0;
		virtual void CBDrawIndexed(GPUResourceHandle handle, U32 count, U32 indexStart = 0, U32 vertStart = 0) = 0;

		void SetWindow(Window* w);
		void SetFrame(U32 frameIndex)
		{
			mFrameIndex = frameIndex;
		}
	protected:
		U32 maxMsaaSamples;
		U32 maxBindings{16};
		U32 maxUniformBuffers{ 1024 };
		U32 mFrameIndex{ 0 };
		Window* window;
		GPUResourceHandle gCommandBufferHandle{0};
		GPUResourceHandle gPipelineStateHandle{0};
		Vector<GPUMemory> mMemories;
		Vector<GPUColorBlendStateCreateInfo> mColorBlendStates;
		Vector<GPUDepthStencilStateCreateInfo> mDepthStencilStates;
		Vector<GPURasterizationStateCreateInfo> mRasterizationStates;
		Vector<GPUMultiSampleStateCreateInfo> mMultiSampleStates;
		Vector<GPUShaderProgramCreateInfo> mShaderPrograms;
		Vector<CommandEncoder> mCommandEncoders;
	};
}