#pragma once
#include "../RenderAPIProtocol.h"
#include "../../Core/Platform.h"
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
		void CreateCommandBuffers(GPUResourceHandle handle, GPUResourceCreateInfo& createInfo, U32 num = 1);
		void CreateFrameBuffers(GPUResourceHandle handle, GPUResourceCreateInfo& createInfo, U32 num = 1);
		void CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo);
		void CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo);

	private:
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		void CreateCommandPool();
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
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

		Vector<VkFramebuffer> mSwapChainFramebuffers;

		Vector<VkSemaphore> mImageAvailableSemaphores;
		Vector<VkSemaphore> mRenderFinishedSemaphores;
		Vector<VkFence> mInFlightFences;

		Vector<CommandBufferVK> mCommandBuffers;
		Vector<ImageVK> mImages;
		Vector<ImageViewVK> mImageViews;

		SwapChainVK mSwapChain;
	};
}