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

	//struct CommandBufferVK {
	//	VulkanContext* ctx;
	//	VkCommandPool pool = VK_NULL_HANDLE;
	//	VkQueue queue = VK_NULL_HANDLE;
	//	VkCommandBuffer commandBuffer;
	//	VkQueue& GetQueue() {
	//		return (VK_NULL_HANDLE == queue ? ctx->graphicsQueue : queue);
	//	}
	//	VkCommandPool& GetPool() {
	//		return (VK_NULL_HANDLE == pool ? ctx->commandPool : pool);
	//	}
	//	void Begin() {
	//		VkCommandBufferAllocateInfo allocInfo{};
	//		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	//		allocInfo.commandPool = GetPool();
	//		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	//		allocInfo.commandBufferCount = 1;

	//		if (vkAllocateCommandBuffers(ctx->device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
	//			LOGERROR("failed to allocate sub command buffer!\n");
	//		}
	//		VkCommandBufferBeginInfo beginInfo{};
	//		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//		beginInfo.flags = 0; // Optional
	//		beginInfo.pInheritanceInfo = nullptr; // Optional

	//		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
	//			LOGERROR("failed to begin recording command buffer!");
	//		}
	//	}
	//	void End(bool submit = true) {
	//		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
	//			LOGERROR("failed to record command buffer!");
	//		}

	//		if (submit) {
	//			Submit();
	//		}
	//	}

	//	void Submit(VkPipelineStageFlags waitMask = 0, VkFence fence = VK_NULL_HANDLE) {
	//		VkSubmitInfo submitInfo = {};
	//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//		submitInfo.commandBufferCount = 1;
	//		submitInfo.pWaitDstStageMask = &waitMask;
	//		submitInfo.pCommandBuffers = &commandBuffer;

	//		vkQueueSubmit(GetQueue(), 1, &submitInfo, fence);
	//		vkQueueWaitIdle(GetQueue());
	//		vkFreeCommandBuffers(ctx->device, GetPool(), 1, &commandBuffer);
	//	}
	//};

	class CommandBufferVK
	{
	public:
		void Create(VkDevice& device, VkCommandBufferAllocateInfo& allocInfo)
		{
			if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
				LOGERROR("failed to allocate command buffer!\n");
		}

		void Begin()
		{	
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
				LOGERROR("failed to begin recording command buffer!");
			}
		}

		void End()
		{
			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				LOGERROR("failed to record command buffer!");
			}
		}

	private:
		VkCommandBuffer commandBuffer;
	};

	class RenderAPIVK : public RenderAPIProtocol
	{
	public:
		void CreateDevice();
		GPUResourceHandle CreateSwapChain(GPUResourceCreateInfo& createInfo, U32 num = 1);
		GPUResourceHandle CreateMainCommandBuffers(U32 num = 1);
		GPUResourceHandle CreateSyncObjects(GPUResourceCreateInfo& createInfo, U32 num = 1);
	private:
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		void CreateCommandPool();
		GPUResourceHandle CreateCommandBuffers(GPUResourceCreateInfo& createInfo, U32 num = 1);
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
		VkSwapchainKHR mSwapChain;
		VkFormat mSwapChainImageFormat;
		VkExtent2D mSwapChainExtent;
		Vector<VkImage> mSwapChainImages;
		Vector<VkImageView> mSwapChainImageViews;

		VkCommandPool mCommandPool;
		//Vector<VkCommandBuffer> mMainCommandBuffers;

		Vector<VkFramebuffer> mSwapChainFramebuffers;

		Vector<VkSemaphore> mImageAvailableSemaphores;
		Vector<VkSemaphore> mRenderFinishedSemaphores;
		Vector<VkFence> mInFlightFences;

		Vector<CommandBufferVK> mCommandBuffers;
	};
}