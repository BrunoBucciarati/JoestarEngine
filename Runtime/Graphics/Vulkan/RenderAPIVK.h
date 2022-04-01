#pragma once
#include "../RenderAPIProtocol.h"
#include "../../Platform/Platform.h"
#include "../../Container/Vector.h"
#include "RenderStructsVK.h"
#include <vulkan/vulkan.h>
namespace Joestar {
	struct QueueFamilyIndices {
		U32 graphicsFamily;
		U32 presentFamily;
		U32 computeFamily;
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		Vector<VkSurfaceFormatKHR> formats;
		Vector<VkPresentModeKHR> presentModes;
	};

	class RenderAPIVK : public RenderAPIProtocol
	{
	public:
		void CreateDevice();
		void CreateSwapChain();
		void CreateSwapChain(GPUSwapChainCreateInfo& createInfo, U32 num) override
		{
			swapChain = createInfo.swapChain;
			swapChain->width = mSwapChain.extent.width;
			swapChain->height = mSwapChain.extent.height;
		}
		void CreateMainCommandBuffers(U32 num = 1);
		void CreateSyncObjects(U32 num = 1);
		void CreateCommandBuffers(GPUResourceHandle handle, GPUCommandBufferCreateInfo& createInfo, U32 num = 1);
		void CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo);
		void CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo);
		void CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo);
		void CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo);
		void CreateIndexBuffer(GPUResourceHandle handle, GPUIndexBufferCreateInfo& createInfo);
		void CreateVertexBuffer(GPUResourceHandle handle, GPUVertexBufferCreateInfo& createInfo);
		void CreateUniformBuffer(GPUResourceHandle handle, GPUUniformBufferCreateInfo& createInfo);
		void CreateRenderPass(GPUResourceHandle handle, GPURenderPassCreateInfo& createInfo);
		void CreateDescriptorPool(U32 num = 1);
		void CreateGraphicsPipelineState(GPUResourceHandle handle, GPUGraphicsPipelineStateCreateInfo& createInfo) override;
		void CreateComputePipelineState(GPUResourceHandle handle, GPUComputePipelineStateCreateInfo& createInfo) override;
		void CreateShader(GPUResourceHandle handle, GPUShaderCreateInfo& createInfo) override;
		void SetUniformBuffer(GPUResourceHandle handle, U8* data, U32 size) override;
		void CreatePipelineLayout(GPUResourceHandle handle, GPUPipelineLayoutCreateInfo& createInfo) override;
		void CreateDescriptorSetLayout(GPUResourceHandle handle, PODVector<DescriptorSetLayoutBinding>& bindings) override;
		void CreateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsCreateInfo& createInfo) override;
		void UpdateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsUpdateInfo& updateInfo) override;
		void QueueSubmit(GPUResourceHandle handle) override;
		void BeginFrame(U32 frameIndex);
		void EndFrame(U32 frameIndex);
		void Present() override;

		//Command Buffer CMDs
		void CBBegin(GPUResourceHandle handle) override;
		void CBEnd(GPUResourceHandle handle) override;
		void CBBeginRenderPass(GPUResourceHandle handle, RenderPassBeginInfo&) override;
		void CBEndRenderPass(GPUResourceHandle handle, GPUResourceHandle) override;
		void CBBindGraphicsPipeline(GPUResourceHandle handle, GPUResourceHandle) override;
		void CBBindComputePipeline(GPUResourceHandle handle, GPUResourceHandle) override;
		void CBBindIndexBuffer(GPUResourceHandle handle, GPUResourceHandle) override;
		void CBBindVertexBuffer(GPUResourceHandle handle, GPUResourceHandle, U32 = 0) override;
		void CBBindDescriptorSets(GPUResourceHandle handle, GPUResourceHandle, GPUResourceHandle, U32=0) override;
		void CBPushConstants(GPUResourceHandle handle, GPUResourceHandle) override;
		void CBDraw(GPUResourceHandle handle, U32 count) override;
		void CBDrawIndexed(GPUResourceHandle handle, U32 count, U32 indexStart = 0, U32 vertStart = 0) override;
	private:
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CreateRenderPass(RenderPassVK* rp, GPURenderPassCreateInfo& createInfo);
		void CreateImage(ImageVK& image, VkImageCreateInfo& createInfo, U32 num = 1);
		void CreateCommandBuffers(CommandBufferVK& cb, VkCommandBufferAllocateInfo& createInfo, U32 num = 1);
		CommandBufferVK GetTempCommandBuffer();
		VkFormat FindSupportedFormat(const Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		void CreateCommandPool();
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices FindQueueFamilies();
		VkSampleCountFlagBits GetMaxUsableSampleCount();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices mQueueFamilyIndices;
		bool bEnableValidationLayers{ true };
		VkInstance mInstance;
		VkPhysicalDevice mPhysicalDevice;
		VkDevice mDevice;
		VkQueue mGraphicsQueue;
		VkQueue mPresentQueue;
		VkQueue mComputeQueue;
		VkSurfaceKHR mSurface;
		VkDebugUtilsMessengerEXT mDebugMessenger;

		VkCommandPool mCommandPool;

		Vector<VkSemaphore> mImageAvailableSemaphores;
		Vector<VkSemaphore> mRenderFinishedSemaphores;
		Vector<VkFence> mInFlightFences;

		Vector<CommandBufferVK*> mCommandBuffers;
		Vector<FrameBufferVK*> mFrameBuffers;
		Vector<ImageVK*> mImages;
		Vector<ImageViewVK*> mImageViews;
		Vector<VertexBufferVK*> mVertexBuffers;
		Vector<IndexBufferVK*> mIndexBuffers;
		Vector<UniformBufferVK*> mUniformBuffers;
		Vector<RenderPassVK*> mRenderPasses;
		Vector<PipelineLayoutVK*> mPipelineLayouts;
		Vector<GraphicsPipelineStateVK*> mGraphicsPipelineStates;
		Vector<ComputePipelineStateVK*> mComputePipelineStates;
		Vector<ShaderVK*> mShaders;
		Vector<DescriptorSetLayoutVK*> mDescriptorSetLayouts;
		Vector<DescriptorSetsVK*> mDescriptorSets;
		VkDescriptorPool mDescriptorPool;

		SwapChainVK mSwapChain;
		U32 mDeviceLocalMemoryTypeIdx{ 0 };
		U32 mImageIndex{ 0 };
	};
}