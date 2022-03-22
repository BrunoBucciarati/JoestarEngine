#pragma once
#include "../RenderAPIProtocol.h"
#include "../../Core/Platform.h"
#include "../../Container/Vector.h"
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
		void CreateCommandBuffers();
		void CreateSyncObjects();
	private:
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		void CreateCommandPool();
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSyncObjects();
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
		VkSwapchainKHR mSwapChain;
		VkFormat mSwapChainImageFormat;
		VkExtent2D mSwapChainExtent;
		Vector<VkImage> mSwapChainImages;
		Vector<VkImageView> mSwapChainImageViews;

		VkCommandPool mCommandPool;
		Vector<VkCommandBuffer> mCommandBuffers;

		Vector<VkFramebuffer> mSwapChainFramebuffers;

		Vector<VkSemaphore> mImageAvailableSemaphores;
		Vector<VkSemaphore> mRenderFinishedSemaphores;
		Vector<VkFence> mInFlightFences;
	};
}