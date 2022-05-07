//#pragma once
//#include "../Graphics/VulkanHeader.h"
//#include "RenderThread.h"
//#include "../Graphics/GPUProgramVulkan.h"
//#include "../Graphics/RenderCommand.h"
//#include "../Container/Vector.h"
//namespace Joestar {
//	const int MAX_FRAMES_IN_FLIGHT = 2;
//	struct SwapChainSupportDetails {
//		VkSurfaceCapabilitiesKHR capabilities;
//		Vector<VkSurfaceFormatKHR> formats;
//		Vector<VkPresentModeKHR> presentModes;
//	};
//	class RenderThreadVulkan : public RenderThread {
//	public:
//		RenderThreadVulkan(EngineContext* ctx, Vector<GFXCommandBuffer*>& cmdBuffers, Vector<GFXCommandBuffer*>& computeBuffers);
//		void ThreadFunc();
//		void InitRenderContext();
//		void DrawFrame(GFXCommandBuffer* cmdBuffer);
//		void DispatchCompute(GFXCommandBuffer* cmdBuffer);
//		bool CheckValidationLayerSupport();
//		void CreateInstance();
//		void CreateSurface();
//		void PickPhysicalDevice();
//		void CreateLogicalDevice();
//		void CreateSwapChain();
//		void CreateImageViews();
//		void CreateCommandPool();
//		//void UpdateUniformBuffer(uint32_t idx);
//		void CreateCommandBuffers();
//		void CreateSyncObjects();
//		void Cleanup();
//		void CleanupSwapChain();
//		void RecreateSwapChain();
//
//		inline void SetFrameBufferResized(bool flag) { framebufferResized = flag; }
//		QueueFamilyIndices FindQueueFamilies();
//		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
//		bool IsDeviceSuitable(VkPhysicalDevice device);
//		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Vector<VkSurfaceFormatKHR>& availableFormats);
//		VkPresentModeKHR ChooseSwapPresentMode(const Vector<VkPresentModeKHR>& availablePresentModes);
//		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
//		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
//		void SetupDebugMessenger();
//		VkFormat FindSupportedFormat(const Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
//		VkFormat FindDepthFormat();
//		bool HasStencilComponent(VkFormat format);
//		VkSampleCountFlagBits GetMaxUsableSampleCount();
//		void ProcessInputVulkan();
//		//void MouseCallback(GLFWwindow* window, double xpos, double ypos);
//		//void ScrollCallback();
//
//	private:
//		GPUProgramVulkan* currentProgram;
//		GLFWwindow* window;
//		VulkanContext vkCtx;
//		Vector<VkSemaphore> imageAvailableSemaphores;
//		Vector<VkSemaphore> renderFinishedSemaphores;
//		Vector<VkFence> inFlightFences;
//		size_t currentFrame = 0;
//		bool framebufferResized = false;
//		VkBuffer indexBuffer;
//		VkDeviceMemory indexBufferMemory;
//		VkDebugUtilsMessengerEXT debugMessenger;
//		VkSampleCountFlagBits msaaSamples;
//	};
//}