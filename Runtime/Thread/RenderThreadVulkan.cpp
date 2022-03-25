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
    const Vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const Vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        //VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        //VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME
    };
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        LOGERROR("validation layer: %s\n", pCallbackData->pMessage);

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT1(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
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
            details.formats.Resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkCtx.surface, &formatCount, details.formats.Buffer());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkCtx.surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.Resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkCtx.surface, &presentModeCount, details.presentModes.Buffer());
        }
        return details;
    }
    bool CheckDeviceExtensionSupport1(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        Vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.Buffer());

        std::set<std::string> requiredExtensions(deviceExtensions.Begin(), deviceExtensions.End());

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
    RenderThreadVulkan::RenderThreadVulkan(EngineContext* ctx, Vector<GFXCommandBuffer*>& cBuffers, Vector<GFXCommandBuffer*>& computeBuffers):
        RenderThread(ctx, cBuffers, computeBuffers)
    {
        vkCtx.physicalDevice = VK_NULL_HANDLE;
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<RenderThreadVulkan*>(glfwGetWindowUserPointer(window));
        app->SetFrameBufferResized(true);//
    }

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

    void RenderThreadVulkan::ThreadFunc() {
        if (!bInit) {
            InitRenderContext();
            bInit = true;
            frameIndex = 0;
        }

        while (!bExit) {
            U32 idx = frameIndex % MAX_CMDLISTS_IN_FLIGHT;
            //always dispatch compute first, then draw
            while (!computeCmdBuffers[idx]->ready || !cmdBuffers[idx]->ready) {
                //busy wait
            }
            DispatchCompute(computeCmdBuffers[idx]);
            DrawFrame(cmdBuffers[idx]);
            cmdBuffers[idx]->ready = false;
            computeCmdBuffers[idx]->ready = false;
            ++frameIndex;
        }
    }

	void RenderThreadVulkan::InitRenderContext() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //GlobalConfig* cfg = Application::GetApplication()->GetEngineContext()->GetSubSystem<GlobalConfig>();
        Application* app = Application::GetApplication();
        GlobalConfig* cfg = app->GetEngineContext()->GetSubsystem<GlobalConfig>();
        uint32_t width = cfg->GetConfig<uint32_t>(CONFIG_WINDOW_WIDTH);
        uint32_t height = cfg->GetConfig<uint32_t>(CONFIG_WINDOW_HEIGHT);
        window = glfwCreateWindow(width, height, "Joestar Engine", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetScrollCallback(window, ScrollCallback);

        GPUProgramVulkan* program = new GPUProgramVulkan();
        currentProgram = program;
        program->SetDevice(&vkCtx);
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        FindQueueFamilies();
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
        Vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Buffer());

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
    void PopulateDebugMessengerCreateInfo1(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void RenderThreadVulkan::SetupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo1(createInfo);

        if (CreateDebugUtilsMessengerEXT1(vkCtx.instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            LOGERROR("failed to set up debug messenger!\n");
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

        Vector<const char*> test;
        test.Push(glfwExtensions[0]);
        test.Push(glfwExtensions[1]);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.Size());
            createInfo.ppEnabledLayerNames = validationLayers.Buffer();
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
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.Size());
            createInfo.ppEnabledLayerNames = validationLayers.Buffer();

            PopulateDebugMessengerCreateInfo1(debugCreateInfo);
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
        for (size_t i = 0; i < vkCtx.swapChainFramebuffers.Size(); i++) {
            vkDestroyFramebuffer(vkCtx.device, vkCtx.swapChainFramebuffers[i], nullptr);
        }

        vkFreeCommandBuffers(vkCtx.device, vkCtx.commandPool, static_cast<uint32_t>(vkCtx.commandBuffers.Size()), vkCtx.commandBuffers.Buffer());

        //vkDestroyPipeline(vkCtx.device, graphicsPipeline, nullptr);
        //vkDestroyPipelineLayout(vkCtx.device, pipelineLayout, nullptr);
        //vkDestroyRenderPass(vkCtx.device, renderPass, nullptr);

        for (size_t i = 0; i < vkCtx.swapChainImageViews.Size(); i++) {
            vkDestroyImageView(vkCtx.device, vkCtx.swapChainImageViews[i], nullptr);
        }
        //for (size_t i = 0; i < vkCtx.swapChainImages.size(); i++) {
        //    vkDestroyBuffer(vkCtx.device, vkCtx.uniformBuffers[i], nullptr);
        //    vkFreeMemory(vkCtx.device, vkCtx.uniformBuffersMemory[i], nullptr);
        //}
        //vkDestroyDescriptorPool(vkCtx.device, vkCtx.descriptorPool, nullptr);
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
    bool extensionsSupported = CheckDeviceExtensionSupport1(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.Empty() && !swapChainSupport.presentModes.Empty();
    }
    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && extensionsSupported && swapChainAdequate;
}

QueueFamilyIndices RenderThreadVulkan::FindQueueFamilies() {
    QueueFamilyIndices indices;
    // Logic to find queue family indices to populate struct with
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkCtx.physicalDevice, &queueFamilyCount, nullptr);

    Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkCtx.physicalDevice, &queueFamilyCount, queueFamilies.Buffer());
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkCtx.physicalDevice, i, vkCtx.surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.computeFamily = i;
        }
        i++;
    }
    vkCtx.queueFamilyIndices = indices;
    return indices;
}

void RenderThreadVulkan::PickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkCtx.instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        LOGERROR("failed to find GPUs with Vulkan support!");
    }
    Vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkCtx.instance, &deviceCount, devices.Buffer());
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

    float queuePriority = 1.0f;
    Vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { vkCtx.queueFamilyIndices.graphicsFamily, vkCtx.queueFamilyIndices.presentFamily };
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.Push(queueCreateInfo);
    }


    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<U32>(queueCreateInfos.Size());
    createInfo.pQueueCreateInfos = queueCreateInfos.Buffer();

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<U32>(deviceExtensions.Size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.Buffer();

    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT floatFeatures{};
    floatFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
    floatFeatures.shaderBufferFloat32AtomicAdd = true; // this allows to perform atomic operations on storage buffers
    floatFeatures.shaderSharedFloat32AtomicAdd = true; 
    floatFeatures.shaderImageFloat32AtomicAdd = true;
    floatFeatures.sparseImageFloat32AtomicAdd = true;
    createInfo.pNext = &floatFeatures;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<U32>(validationLayers.Size());
        createInfo.ppEnabledLayerNames = validationLayers.Buffer();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
    if (vkCreateDevice(vkCtx.physicalDevice, &createInfo, nullptr, &vkCtx.device) != VK_SUCCESS) {
        LOGERROR("failed to create logical vkCtx.device!");
    }

    vkGetDeviceQueue(vkCtx.device, vkCtx.queueFamilyIndices.graphicsFamily, 0, &vkCtx.graphicsQueue);
    vkGetDeviceQueue(vkCtx.device, vkCtx.queueFamilyIndices.presentFamily, 0, &vkCtx.presentQueue);
    vkGetDeviceQueue(vkCtx.device, vkCtx.queueFamilyIndices.computeFamily, 0, &vkCtx.computeQueue);
}

VkSurfaceFormatKHR RenderThreadVulkan::ChooseSwapSurfaceFormat(const Vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR RenderThreadVulkan::ChooseSwapPresentMode(const Vector<VkPresentModeKHR>& availablePresentModes) {
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

    uint32_t queueFamilyIndices[] = { vkCtx.queueFamilyIndices.graphicsFamily, vkCtx.queueFamilyIndices.presentFamily };

    if (vkCtx.queueFamilyIndices.graphicsFamily != vkCtx.queueFamilyIndices.presentFamily) {
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
    
    vkCtx.swapChainImages.Resize(imageCount);
    vkGetSwapchainImagesKHR(vkCtx.device, vkCtx.swapChain, &imageCount, vkCtx.swapChainImages.Buffer());
    vkCtx.swapChainImageFormat = surfaceFormat.format;
    vkCtx.swapChainExtent = extent;
}

void RenderThreadVulkan::CreateImageViews() {
    vkCtx.swapChainImageViews.Resize(vkCtx.swapChainImages.Size());
    for (size_t i = 0; i < vkCtx.swapChainImages.Size(); i++) {
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
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = vkCtx.queueFamilyIndices.graphicsFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
    //create sub command pool
    if (vkCreateCommandPool(vkCtx.device, &poolInfo, nullptr, &(currentProgram->subCommandPool)) != VK_SUCCESS) {
        LOGERROR("failed to create command pool!");
    }

    //poolInfo.flags = 0; // Optional
    if (vkCreateCommandPool(vkCtx.device, &poolInfo, nullptr, &(vkCtx.commandPool)) != VK_SUCCESS) {
        LOGERROR("failed to create command pool!");
    }

}
 
void RenderThreadVulkan::CreateCommandBuffers() {
    vkCtx.commandBuffers.Resize(vkCtx.swapChainImageViews.Size());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vkCtx.commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)vkCtx.commandBuffers.Size();

    if (vkAllocateCommandBuffers(vkCtx.device, &allocInfo, vkCtx.commandBuffers.Buffer()) != VK_SUCCESS) {
        LOGERROR("failed to allocate command buffers!");
    }
}

void RenderThreadVulkan::CreateSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAvailableSemaphores.Resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.Resize(MAX_FRAMES_IN_FLIGHT);

    inFlightFences.Resize(MAX_FRAMES_IN_FLIGHT);
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
#define GetGLKey(KEY) \
        if (glfwGetKey(window, GLFW_##KEY) == GLFW_PRESS) { \
            inputFlag |= 1 << KEY; \
        }
void RenderThreadVulkan::ProcessInputVulkan() {
    uint32_t inputFlag = 0;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        inputFlag |= 1 << KEY_ESCAPE;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetWindowShouldClose(window, true);
    }
        GetGLKey(KEY_W)
        GetGLKey(KEY_S)
        GetGLKey(KEY_A)
        GetGLKey(KEY_D)
        GetGLKey(KEY_UP)
        GetGLKey(KEY_DOWN)
        GetGLKey(KEY_LEFT)
        GetGLKey(KEY_RIGHT)
    Application::GetApplication()->GetSubSystem<HID>()->SetKeyboardInputs(inputFlag);
    Application::GetApplication()->GetSubSystem<HID>()->SetMouseInputs(xoffset, yoffset, scrollX, scrollY);
    //reset
    xoffset = yoffset = scrollX = scrollY = 0.f;
}


void RenderThreadVulkan::DispatchCompute(GFXCommandBuffer* cmdBuffer) {
    currentProgram->ExecuteComputeCommand(cmdBuffer);
}

void RenderThreadVulkan::DrawFrame(GFXCommandBuffer* cmdBuffer) {
    ProcessInputVulkan();
    glfwPollEvents();
    //if (glfwWindowShouldClose(window)) {
    //    return;
    //}
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

    //no incoming draw call
    currentProgram->ExecuteRenderCommand(cmdBuffer, imageIndex);
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
    CreateCommandBuffers();
}

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

VkFormat RenderThreadVulkan::FindSupportedFormat(const Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
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