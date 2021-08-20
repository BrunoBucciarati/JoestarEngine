#pragma once
#include "../Graphics/VulkanHeader.h"
#include "RenderThread.h"
#include <vector>
#include "../Graphics/GPUProgramVulkan.h"
namespace Joestar {
	const int MAX_FRAMES_IN_FLIGHT = 2;
	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;
	};
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	class RenderThreadVulkan : public RenderThread {
	public:
		RenderThreadVulkan();
		void InitRenderContext();
		void DrawFrame();
		bool CheckValidationLayerSupport();
		void CreateInstance();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();
		//void CreateRenderPass();
		//void CreateGraphicsPipeline();
		void CreateFrameBuffers();
		void CreateCommandPool();
		void CreateVertexBuffer();
		void CreateUniformBuffers();
		void UpdateUniformBuffer(uint32_t idx);
		void CreateCommandBuffers();
		void CreateSyncObjects();
		void Cleanup();
		void CleanupSwapChain();
		void RecreateSwapChain();
		void CreateDescriptorSetLayout();

		inline void SetFrameBufferResized(bool flag) { framebufferResized = flag; }
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void SetupDebugMessenger();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateDepthResources();
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat FindDepthFormat();
		bool HasStencilComponent(VkFormat format);
		VkSampleCountFlagBits GetMaxUsableSampleCount();
		void CreateColorResources();
		void ProcessInputVulkan();

	private:
		GPUProgramVulkan* currentProgram;
		GLFWwindow* window;
		VulkanContext vkCtx;
/*		VkRenderPass renderPass;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;*/ 
		std::vector<VkFramebuffer> swapChainFramebuffers;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		size_t currentFrame = 0;
		bool framebufferResized = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;
		VkImage colorImage;
		VkDeviceMemory colorImageMemory;
		VkImageView colorImageView;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;


		// only for test!!!!!!!
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void createUniformBuffers();
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	};
}