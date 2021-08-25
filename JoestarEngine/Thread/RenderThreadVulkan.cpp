#pragma once
#include "RenderThreadVulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vector>
#include <set>
#include "../Misc/GlobalConfig.h"
#include "../Misc/Application.h"
#include "../IO/Log.h"
#include "../Graphics/GPUProgram.h"
#include <optional>
#include <algorithm>
#include <cstdint>
#include "../IO/HID.h"
namespace Joestar {
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        LOGERROR("validation layer: %s\n", pCallbackData->pMessage);

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    SwapChainSupportDetails RenderThreadVulkan::QuerySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vkCtx.surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkCtx.surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkCtx.surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkCtx.surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkCtx.surface, &presentModeCount, details.presentModes.data());
        }
        return details;
    }
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
    RenderThreadVulkan::RenderThreadVulkan() {
        vkCtx.physicalDevice = VK_NULL_HANDLE;
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<RenderThreadVulkan*>(glfwGetWindowUserPointer(window));
        app->SetFrameBufferResized(true);//
    }

    //void RenderThreadVulkan::MouseCallback(GLFWwindow* window, double xpos, double ypos) {

    //}
    bool firstMouse = true;
    float lastX, lastY, xoffset, yoffset, scrollX, scrollY;
    void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        xoffset = xpos - lastX;
        yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
    }

    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        scrollX = xoffset;
        scrollY = yoffset;
    }

	void RenderThreadVulkan::InitRenderContext() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //GlobalConfig* cfg = Application::GetApplication()->GetEngineContext()->GetSubSystem<GlobalConfig>();
        Application* app = Application::GetApplication();
        GlobalConfig* cfg = app->GetEngineContext()->GetSubSystem<GlobalConfig>();
        uint32_t width = cfg->GetConfig<uint32_t>("WINDOW_WIDTH");
        uint32_t height = cfg->GetConfig<uint32_t>("WINDOW_HEIGHT");
        window = glfwCreateWindow(width, height, "Joestar Engine", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetScrollCallback(window, ScrollCallback);

        GPUProgramVulkan* program = static_cast<GPUProgramVulkan*>(GPUProgram::CreateProgram());
        currentProgram = program;
        program->SetDevice(&vkCtx);
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateCommandPool();
        CreateCommandBuffers();
        //program->GetPipelineContext();
        CreateSyncObjects();
	}
	bool RenderThreadVulkan::CheckValidationLayerSupport() {
        uint32_t layerCount = 0;
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void RenderThreadVulkan::SetupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(vkCtx.instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            LOGERROR("failed to set up debug messenger!");
        }
    }
    void RenderThreadVulkan::CreateInstance() {
        if (enableValidationLayers && !CheckValidationLayerSupport()) {
            LOGERROR("validation layers requested, but not available!\n");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Joestar Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Joestar Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        VkResult result = vkCreateInstance(&createInfo, nullptr, &vkCtx.instance);
        if (result != VK_SUCCESS) {
            LOGERROR("failed to create vkCtx.instance!");
        }

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &vkCtx.instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void RenderThreadVulkan::CreateSurface() {
        if (glfwCreateWindowSurface(vkCtx.instance, window, nullptr, &vkCtx.surface) != VK_SUCCESS) {
            LOGERROR("failed to create window vkCtx.surface!");
        }
    }

    void RenderThreadVulkan::CleanupSwapChain() {
        for (size_t i = 0; i < vkCtx.swapChainFramebuffers.size(); i++) {
            vkDestroyFramebuffer(vkCtx.device, vkCtx.swapChainFramebuffers[i], nullptr);
        }

        vkFreeCommandBuffers(vkCtx.device, vkCtx.commandPool, static_cast<uint32_t>(vkCtx.commandBuffers.size()), vkCtx.commandBuffers.data());

        //vkDestroyPipeline(vkCtx.device, graphicsPipeline, nullptr);
        //vkDestroyPipelineLayout(vkCtx.device, pipelineLayout, nullptr);
        //vkDestroyRenderPass(vkCtx.device, renderPass, nullptr);

        for (size_t i = 0; i < vkCtx.swapChainImageViews.size(); i++) {
            vkDestroyImageView(vkCtx.device, vkCtx.swapChainImageViews[i], nullptr);
        }
        //for (size_t i = 0; i < vkCtx.swapChainImages.size(); i++) {
        //    vkDestroyBuffer(vkCtx.device, vkCtx.uniformBuffers[i], nullptr);
        //    vkFreeMemory(vkCtx.device, vkCtx.uniformBuffersMemory[i], nullptr);
        //}
        vkDestroyDescriptorPool(vkCtx.device, vkCtx.descriptorPool, nullptr);
        vkDestroySwapchainKHR(vkCtx.device, vkCtx.swapChain, nullptr);
    }


void RenderThreadVulkan::Cleanup() {
    CleanupSwapChain();
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vkCtx.device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(vkCtx.device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(vkCtx.device, inFlightFences[i], nullptr);
    }
    if (currentProgram) {
        currentProgram->Clean();
    }
    vkDestroyCommandPool(vkCtx.device, vkCtx.commandPool, nullptr);
    vkDestroyDevice(vkCtx.device, nullptr);

    vkDestroySurfaceKHR(vkCtx.instance, vkCtx.surface, nullptr);
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(vkCtx.instance, debugMessenger, nullptr);
    }
    vkDestroyInstance(vkCtx.instance, nullptr);
    glfwDestroyWindow(window);

    glfwTerminate();
}

bool RenderThreadVulkan::IsDeviceSuitable(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && extensionsSupported && swapChainAdequate;
}

QueueFamilyIndices RenderThreadVulkan::FindQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    // Logic to find queue family indices to populate struct with
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vkCtx.surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            if (presentSupport) break;
        }
        i++;
    }
    return indices;
}

void RenderThreadVulkan::PickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkCtx.instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        LOGERROR("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkCtx.instance, &deviceCount, devices.data());
    for (const auto& device : devices) {
        if (IsDeviceSuitable(device)) {
            vkCtx.physicalDevice = device;
            msaaSamples = GetMaxUsableSampleCount();
            break;
        }
    }

    if (vkCtx.physicalDevice == VK_NULL_HANDLE) {
        LOGERROR("failed to find a suitable GPU!");
    }
}

void RenderThreadVulkan::CreateLogicalDevice() {
    QueueFamilyIndices indices = FindQueueFamilies(vkCtx.physicalDevice);

    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }


    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
    if (vkCreateDevice(vkCtx.physicalDevice, &createInfo, nullptr, &vkCtx.device) != VK_SUCCESS) {
        LOGERROR("failed to create logical vkCtx.device!");
    }

    vkGetDeviceQueue(vkCtx.device, indices.graphicsFamily, 0, &vkCtx.graphicsQueue);
    vkGetDeviceQueue(vkCtx.device, indices.presentFamily, 0, &vkCtx.presentQueue);
}

VkSurfaceFormatKHR RenderThreadVulkan::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR RenderThreadVulkan::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D RenderThreadVulkan::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::min(std::max(actualExtent.width, capabilities.minImageExtent.width), capabilities.maxImageExtent.width);
        actualExtent.height = std::min(std::max(actualExtent.height, capabilities.minImageExtent.height), capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void RenderThreadVulkan::CreateSwapChain() {
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(vkCtx.physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkCtx.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(vkCtx.physicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vkCtx.device, &createInfo, nullptr, &vkCtx.swapChain) != VK_SUCCESS) {
        LOGERROR("failed to create swap chain!");
    }
    vkGetSwapchainImagesKHR(vkCtx.device, vkCtx.swapChain, &imageCount, nullptr);
    
    vkCtx.swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkCtx.device, vkCtx.swapChain, &imageCount, vkCtx.swapChainImages.data());
    vkCtx.swapChainImageFormat = surfaceFormat.format;
    vkCtx.swapChainExtent = extent;
}

void RenderThreadVulkan::CreateImageViews() {
    vkCtx.swapChainImageViews.resize(vkCtx.swapChainImages.size());
    for (size_t i = 0; i < vkCtx.swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = vkCtx.swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = vkCtx.swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(vkCtx.device, &createInfo, nullptr, &vkCtx.swapChainImageViews[i]) != VK_SUCCESS) {
            LOGERROR("failed to create image views!");
        }
    }
}

void RenderThreadVulkan::CreateCommandPool() {
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(vkCtx.physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = 0; // Optional
    
    if (vkCreateCommandPool(vkCtx.device, &poolInfo, nullptr, &(vkCtx.commandPool)) != VK_SUCCESS) {
        LOGERROR("failed to create command pool!");
    }
}
 
void RenderThreadVulkan::CreateCommandBuffers() {
    vkCtx.commandBuffers.resize(vkCtx.swapChainImageViews.size());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vkCtx.commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)vkCtx.commandBuffers.size();

    if (vkAllocateCommandBuffers(vkCtx.device, &allocInfo, vkCtx.commandBuffers.data()) != VK_SUCCESS) {
        LOGERROR("failed to allocate command buffers!");
    }
}

void RenderThreadVulkan::CreateSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vkCtx.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(vkCtx.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(vkCtx.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            LOGERROR("failed to create synchronization objects for a frame!");
        }
    }
}

void RenderThreadVulkan::ProcessInputVulkan() {
    uint32_t inputFlag = 0;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        inputFlag |= 1 << KEY_ESCAPE;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        inputFlag |= 1 << KEY_W;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        inputFlag |= 1 << KEY_S;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        inputFlag |= 1 << KEY_A;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        inputFlag |= 1 << KEY_D;
    }
    Application::GetApplication()->GetSubSystem<HID>()->SetKeyboardInputs(inputFlag);
    Application::GetApplication()->GetSubSystem<HID>()->SetMouseInputs(xoffset, yoffset, scrollX, scrollY);
    //reset
    xoffset = yoffset = scrollX = scrollY = 0.f;
}


void RenderThreadVulkan::DrawFrame(std::vector<RenderCommand>& cmdBuffer, uint16_t cmdIdx) {
    ProcessInputVulkan();
    glfwPollEvents();
    //if (glfwWindowShouldClose(window)) {
    //    return;
    //}

    //no incoming draw call
    if (cmdIdx == 0) return;
    currentProgram->ExecuteRenderCommand(cmdBuffer, cmdIdx);
    if (vkCtx.commandBuffers.empty()) return;
    vkWaitForFences(vkCtx.device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t >::max());
    vkResetFences(vkCtx.device, 1, &inFlightFences[currentFrame]);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(vkCtx.device, vkCtx.swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        LOGERROR("failed to acquire swap chain image!");
    }
    currentProgram->UpdateUniformBuffer(imageIndex);
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCtx.commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vkCtx.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        LOGERROR("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { vkCtx.swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional
    result = vkQueuePresentKHR(vkCtx.presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        LOGERROR("failed to acquire swap chain image!");
    }

    currentFrame++;
    currentFrame %= MAX_FRAMES_IN_FLIGHT;
}

void RenderThreadVulkan::RecreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(vkCtx.device);

    CleanupSwapChain();

    CreateSwapChain();
    CreateImageViews();
    //CreateDescriptorPool();
    CreateCommandBuffers();
}

//void RenderThreadVulkan::UpdateUniformBuffer(uint32_t currentImage) {
//    static auto startTime = std::chrono::high_resolution_clock::now();
//    UniformBufferObject ubo{};
//    ubo.model = Matrix4x4f::identity;
//    ubo.view.LookAt(Vector3f(0.0f, 0.0f, -2.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
//    ubo.proj.SetPerspective(45.f, vkCtx.swapChainExtent.width / (float)vkCtx.swapChainExtent.height, 0.1f, 10.0f);
//    //auto currentTime = std::chrono::high_resolution_clock::now();
//    //float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
//    Vector4f test1 = ubo.view.MultiplyVector4(Vector4f(0.5, 0.5, 0, 1));
//    //test1.z = -test1.z;
//    Vector4f test = ubo.proj.MultiplyVector4(test1);
//
//    void* data;
//    vkMapMemory(vkCtx.device, vkCtx.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
//    memcpy(data, &ubo, sizeof(ubo));
//    vkUnmapMemory(vkCtx.device, vkCtx.uniformBuffersMemory[currentImage]);
//}

VkFormat RenderThreadVulkan::FindDepthFormat() {
    return FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool RenderThreadVulkan::HasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat RenderThreadVulkan::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(vkCtx.physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    LOGERROR("failed to find supported format!");
}
VkSampleCountFlagBits RenderThreadVulkan::GetMaxUsableSampleCount() {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vkCtx.physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    //no need for msaa right now
    //if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    //if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    //if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    //if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    //if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    //if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_2_BIT;
}

}