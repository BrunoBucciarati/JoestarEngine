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
#include "../Base/ObjectDefines.h"

#define VK_CHECK(f) \
	VkResult result = f; \
	if (result != VK_SUCCESS) LOGERROR("[VK_ERROR]:%s", #f);


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
	VkDescriptorPool descriptorPool;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
};