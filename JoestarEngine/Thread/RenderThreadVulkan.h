#pragma once
#include "../Graphics/VulkanHeader.h"
#include "RenderThread.h"
#include <vector>
#include "../Graphics/GPUProgramVulkan.h"
#include "../Graphics/RenderCommand.h"
namespace Joestar {
	const int MAX_FRAMES_IN_FLIGHT = 2;
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	class RenderThreadVulkan : public RenderThread {
	public:
		RenderThreadVulkan(Vector<GFXCommandBuffer*>& cmdBuffers, Vector<GFXCommandBuffer*>& computeBuffers);
		void ThreadFunc();
		void InitRenderContext();
		void DrawFrame(GFXCommandBuffer* cmdBuffer);
		void DispatchCompute(GFXCommandBuffer* cmdBuffer);
		bool CheckValidationLayerSupport();
		void CreateInstance();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateCommandPool();
		//void UpdateUniformBuffer(uint32_t idx);
		void CreateCommandBuffers();
		void CreateSyncObjects();
		void Cleanup();
		void CleanupSwapChain();
		void RecreateSwapChain();

		inline void SetFrameBufferResized(bool flag) { framebufferResized = flag; }
		QueueFamilyIndices FindQueueFamilies();
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void SetupDebugMessenger();
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat FindDepthFormat();
		bool HasStencilComponent(VkFormat format);
		VkSampleCountFlagBits GetMaxUsableSampleCount();
		void ProcessInputVulkan();
		//void MouseCallback(GLFWwindow* window, double xpos, double ypos);
		//void ScrollCallback();

	private:
		GPUProgramVulkan* currentProgram;
		GLFWwindow* window;
		VulkanContext vkCtx;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		size_t currentFrame = 0;
		bool framebufferResized = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkSampleCountFlagBits msaaSamples;
	};
}