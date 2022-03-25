#pragma once
#include <glad/glad.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>
#include "../Container/Vector.h"
#include "../IO/Log.h"
#include "../Platform/Platform.h"
#include "../Core/ObjectDefines.h"
#define VK_CHECK(f) \
	if (VK_SUCCESS != f) {LOGERROR("[VK_ERROR:%d]%s\n", f, #f)};

namespace Joestar {

	struct QueueFamilyIndices {
		U32 graphicsFamily;
		U32 presentFamily;
		U32 computeFamily;
	};

	struct VulkanContext {
		VkInstance instance;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkQueue computeQueue;
		VkSurfaceKHR surface;
		VkSwapchainKHR swapChain;
		VkCommandPool commandPool;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		QueueFamilyIndices queueFamilyIndices;
		Joestar::Vector<VkImage> swapChainImages;
		Joestar::Vector<VkImageView> swapChainImageViews;
		//VkDescriptorPool descriptorPool;
		Joestar::Vector<VkFramebuffer> swapChainFramebuffers;
		Joestar::Vector<VkCommandBuffer> commandBuffers;
	};
}