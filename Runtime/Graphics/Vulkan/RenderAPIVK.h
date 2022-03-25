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
		void CreateSwapChain(GPUSwapChainCreateInfo& createInfo, U32 num = 1);
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

		Vector<CommandBufferVK> mCommandBuffers;
		Vector<FrameBufferVK> mFrameBuffers;
		Vector<ImageVK> mImages;
		Vector<ImageViewVK> mImageViews;
		Vector<VertexBufferVK> mVertexBuffers;
		Vector<IndexBufferVK> mIndexBuffers;
		Vector<UniformBufferVK> mUniformBuffers;
		Vector<RenderPassVK> mRenderPasses;
		VkDescriptorPool mDescriptorPool;

		SwapChainVK mSwapChain;
		U32 mDeviceLocalMemoryTypeIdx{ 0 };
	};
}