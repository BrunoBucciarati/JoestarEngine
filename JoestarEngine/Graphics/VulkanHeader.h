#pragma once
#include <glad/glad.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>
#include <vector>
#include "../IO/Log.h"
#include "../Base/Platform.h"
#include "../Base/ObjectDefines.h"
#define VK_CHECK(f) \
	if (VK_SUCCESS != f) {LOGERROR("[VK_ERROR:%d]%s\n", f, #f)};


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
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	//VkDescriptorPool descriptorPool;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
};