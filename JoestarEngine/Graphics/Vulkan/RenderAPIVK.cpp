#include "RenderAPIVK.h"
#include "../../Container/Vector.h"
#include "../../Container/HashSet.h"
#include "../../Math/MathDefs.h"
#include "../Window.h"
#include <vulkan/vulkan_win32.h>
#include "RenderEnumsVK.h"

namespace Joestar {
    VkResult globalResult;
#define VK_CHECK(fn) \
    globalResult = fn; \
    if (globalResult != VK_SUCCESS) \
    { \
        LOGERROR("[VK ERROR=%d] %s\n", globalResult, #fn); \
    }

    const Vector<const char*> DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
        //VK_USE_PLATFORM_WIN32_KHR
    };
    const Vector<const char*> ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        LOGERROR("validation layer: %s\n", pCallbackData->pMessage);

        return VK_FALSE;
    }
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
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

    U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& device) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        LOGERROR("failed to find suitable memory type!");
    }

    bool CheckValidationLayerSupport() {
        U32 layerCount = 0;
        Vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Buffer());

        for (const char* layerName : ValidationLayers) {
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

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        Vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.Buffer());

        HashSet<String> requiredExtensions;
        for (auto& ext : DeviceExtensions)
        {
            requiredExtensions.Insert(ext);
        }

        for (const auto& extension : availableExtensions) {
            requiredExtensions.Erase(extension.extensionName);
        }

        return requiredExtensions.Empty();
    }

    bool RenderAPIVK::IsDeviceSuitable(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.Empty() && !swapChainSupport.presentModes.Empty();
        }
        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && extensionsSupported && swapChainAdequate;
    }


    SwapChainSupportDetails RenderAPIVK::QuerySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

        U32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.Resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.Buffer());
        }

        U32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.Resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.Buffer());
        }
        return details;
    }

    void RenderAPIVK::CreateDevice()
    {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        FindQueueFamilies();
        CreateLogicalDevice();
    }


    QueueFamilyIndices RenderAPIVK::FindQueueFamilies() {
        QueueFamilyIndices indices;
        // Logic to find queue family indices to populate struct with
        U32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);

        Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.Buffer());
        U32 i = 0;
        for (const auto& queueFamily : queueFamilies) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.computeFamily = i;
            }
            ++i;
        }
        mQueueFamilyIndices = indices;
        return indices;
    }

    void RenderAPIVK::SetupDebugMessenger() {
        if (!bEnableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);

        VK_CHECK(CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger));
    }

    void RenderAPIVK::CreateSurface() {
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hinstance = window->GetAppInstance();
        createInfo.hwnd = window->GetMainWindow();

        VK_CHECK(vkCreateWin32SurfaceKHR(mInstance, &createInfo, nullptr, &mSurface));
    }

    void RenderAPIVK::CreateInstance() {
        if (bEnableValidationLayers && !CheckValidationLayerSupport()) {
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

        Vector<const char*> surfaceExtension{
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
        };
        createInfo.enabledExtensionCount = surfaceExtension.Size();
        createInfo.ppEnabledExtensionNames = surfaceExtension.Buffer();
        if (bEnableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.Size());
            createInfo.ppEnabledLayerNames = ValidationLayers.Buffer();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }
        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &mInstance))

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (bEnableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.Size());
            createInfo.ppEnabledLayerNames = ValidationLayers.Buffer();

            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &mInstance));
    }

    VkSampleCountFlagBits RenderAPIVK::GetMaxUsableSampleCount() {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &physicalDeviceProperties);

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

    void RenderAPIVK::PickPhysicalDevice()
    {
        U32 deviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            LOGERROR("failed to find GPUs with Vulkan support!");
        }
        Vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.Buffer());
        for (const auto& device : devices)
        {
            if (IsDeviceSuitable(device))
            {
                mPhysicalDevice = device;
                msaaSamples = GetMaxUsableSampleCount();
                break;
            }
        }

        if (mPhysicalDevice == VK_NULL_HANDLE) {
            LOGERROR("failed to find a suitable GPU!");
        }
    }

	void RenderAPIVK::CreateLogicalDevice()
	{
        PickPhysicalDevice();
        float queuePriority = 1.0f;
        Vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        HashSet<U32> uniqueQueueFamilies = { mQueueFamilyIndices.graphicsFamily, mQueueFamilyIndices.presentFamily };
        for (U32 queueFamily : uniqueQueueFamilies) {
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
        createInfo.queueCreateInfoCount = queueCreateInfos.Size();
        createInfo.pQueueCreateInfos = queueCreateInfos.Buffer();

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = DeviceExtensions.Size();
        createInfo.ppEnabledExtensionNames = DeviceExtensions.Buffer();

        VkPhysicalDeviceShaderAtomicFloatFeaturesEXT floatFeatures{};
        floatFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
        floatFeatures.shaderBufferFloat32AtomicAdd = true; // this allows to perform atomic operations on storage buffers
        floatFeatures.shaderSharedFloat32AtomicAdd = true;
        floatFeatures.shaderImageFloat32AtomicAdd = true;
        floatFeatures.sparseImageFloat32AtomicAdd = true;
        createInfo.pNext = &floatFeatures;

        if (bEnableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<U32>(ValidationLayers.Size());
            createInfo.ppEnabledLayerNames = ValidationLayers.Buffer();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }
        if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
            LOGERROR("failed to create logical vkCtx.device!");
        }

        vkGetDeviceQueue(mDevice, mQueueFamilyIndices.graphicsFamily, 0, &mGraphicsQueue);
        vkGetDeviceQueue(mDevice, mQueueFamilyIndices.presentFamily, 0, &mPresentQueue);
        vkGetDeviceQueue(mDevice, mQueueFamilyIndices.computeFamily, 0, &mComputeQueue);

	}

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR ChooseSwapPresentMode(const Vector<VkPresentModeKHR>& availablePresentModes) {
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window* window) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        }
        else {

            VkExtent2D actualExtent = {
                window->GetWidth(),
                window->GetHeight()
            };

            actualExtent.width = Min(Max(actualExtent.width, capabilities.minImageExtent.width), capabilities.maxImageExtent.width);
            actualExtent.height = Min(Max(actualExtent.height, capabilities.minImageExtent.height), capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

	void RenderAPIVK::CreateSwapChain(GPUSwapChainCreateInfo& ci, U32 num)
	{
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(mPhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);

        U32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = mSurface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        U32 queueFamilyIndices[] = { mQueueFamilyIndices.graphicsFamily, mQueueFamilyIndices.presentFamily };

        if (mQueueFamilyIndices.graphicsFamily != mQueueFamilyIndices.presentFamily) {
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

        VK_CHECK(vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain.swapChain));
        VK_CHECK(vkGetSwapchainImagesKHR(mDevice, mSwapChain.swapChain, &imageCount, nullptr));

        mSwapChain.images.Resize(imageCount);
        VK_CHECK(vkGetSwapchainImagesKHR(mDevice, mSwapChain.swapChain, &imageCount, mSwapChain.images.Buffer()));
        mSwapChain.format = surfaceFormat.format;
        mSwapChain.extent = extent;

        mSwapChain.imageViews.Resize(mSwapChain.images.Size());
        for (U32 i = 0; i < mSwapChain.GetImageCount(); ++i)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = mSwapChain.images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = mSwapChain.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            VK_CHECK(vkCreateImageView(mDevice, &createInfo, nullptr, &mSwapChain.imageViews[i]));
        }
	}

    void RenderAPIVK::CreateCommandPool()
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = mQueueFamilyIndices.graphicsFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

        VK_CHECK(vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool));
    }

    void RenderAPIVK::CreateCommandBuffers(GPUResourceHandle handle, GPUResourceCreateInfo& createInfo, U32 num)
    {
        if (handle + 1 > mCommandBuffers.Size())
            mCommandBuffers.Resize(handle + 1);
        CommandBufferVK& cb = mCommandBuffers[handle];

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = mCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = num;
        cb.Create(mDevice, allocInfo);
    }

    void RenderAPIVK::CreateMainCommandBuffers(U32 num)
    {
        CreateCommandPool();

        GPUResourceCreateInfo ci;
        return CreateCommandBuffers(0, ci, num);
    }

    void RenderAPIVK::CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo)
    {
        if (handle + 1 > mImages.Size())
            mImages.Resize(handle + 1);
        ImageVK& image = mImages[handle];
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = GetImageType(createInfo.type);
        imageInfo.extent.width = createInfo.width;
        imageInfo.extent.height = createInfo.height;
        imageInfo.extent.depth = createInfo.depth;
        imageInfo.mipLevels = createInfo.mipLevels;
        imageInfo.arrayLayers = createInfo.layer;
        imageInfo.format = GetImageFormat(createInfo.format);
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = GetImageUsageBits(createInfo.usage);;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VkSampleCountFlagBits(createInfo.samples);
        //imageInfo.flags = viewType == VK_IMAGE_VIEW_TYPE_CUBE ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0; // Optional  
        imageInfo.flags = 0; // Optional  

        CreateImage(image, imageInfo, createInfo.num);

    }

    void RenderAPIVK::CreateImage(ImageVK& image, VkImageCreateInfo& imageInfo, U32 num)
    {
        image.Create(mDevice, imageInfo, num);

        U32 memoryTypeIdx = FindMemoryType(image.memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mPhysicalDevice);
        image.AllocMemory(mDevice, memoryTypeIdx);
    }


    void RenderAPIVK::CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo)
    {
        if (handle + 1 > mImageViews.Size())
            mImageViews.Resize(handle + 1);
        ImageViewVK& imageView = mImageViews[handle];
        ImageVK image = mImages[createInfo.imageHandle];
        imageView.image = &image;
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType = GetImageViewType(createInfo.type);
        viewInfo.format = GetImageFormat(createInfo.format);
        viewInfo.subresourceRange.aspectMask = GetImageAspectBits(createInfo.aspectBits);
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = createInfo.mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = createInfo.baseLayer;
        viewInfo.subresourceRange.layerCount = createInfo.layer;
        imageView.Create(mDevice, viewInfo, createInfo.num);
    }

    //void GPUProgramVulkan::CreateColorResources(RenderPassVK* pass) {
    //    VkFormat colorFormat = vkCtxPtr->swapChainImageFormat;
    //    pass->fb->colorImage = new ImageVK{
    //        vkCtxPtr, vkCtxPtr->swapChainExtent.width, vkCtxPtr->swapChainExtent.height, 1, pass->msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    //    };
    //    CommandBufferVK cb{ vkCtxPtr };
    //    cb.Begin();
    //    pass->fb->colorImage->Create();
    //    pass->fb->colorImage->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT, cb);
    //    cb.End();
    //}

    //void GPUProgramVulkan::CreateDepthResources(RenderPassVK* pass) {
    //    VkFormat depthFormat = FindDepthFormat();
    //    pass->fb->depthImage = new ImageVK{
    //        vkCtxPtr,
    //        vkCtxPtr->swapChainExtent.width, vkCtxPtr->swapChainExtent.height,
    //        1, pass->msaaSamples,  depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    //    };
    //    pass->fb->depthImage->Create();
    //    CommandBufferVK cb{ vkCtxPtr };
    //    cb.Begin();
    //    pass->fb->depthImage->CreateImageView(VK_IMAGE_ASPECT_DEPTH_BIT, cb);
    //    pass->fb->depthImage->TransitionImageLayout(cb, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);
    //    cb.End();
    //}

    void RenderAPIVK::CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo)
    {

    }

    void RenderAPIVK::CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo)
    {
        if (mSwapChain.frameBuffer)
            return;
        mSwapChain.frameBuffer = JOJO_NEW(FrameBufferVK);
        if (mFrameBuffers.Empty())
            mFrameBuffers.Resize(1);
        mFrameBuffers[0] = *mSwapChain.frameBuffer;

        mSwapChain.frameBuffer->frameBuffers.Resize(mSwapChain.imageViews.Size());
        VkFormat depthFormat = FindSupportedFormat(
            { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );

        ImageVK* depthImage = JOJO_NEW(ImageVK);
        VkImageCreateInfo depthImgCreateInfo{};
        depthImgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        depthImgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        depthImgCreateInfo.extent.width = mSwapChain.extent.width;
        depthImgCreateInfo.extent.height = mSwapChain.extent.height;
        depthImgCreateInfo.format = depthFormat;
        depthImgCreateInfo.mipLevels = 1;
        depthImgCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImgCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImgCreateInfo.samples = (VkSampleCountFlagBits)msaaSamples;
        CreateImage(*depthImage, depthImgCreateInfo, mSwapChain.GetImageCount());

        ImageViewVK* depthStencilView = JOJO_NEW(ImageViewVK);
        depthStencilView->image = depthImage;
        mSwapChain.frameBuffer->depthStencilAttachment = depthStencilView;

        VkImageViewCreateInfo depthViewCreateInfo{};
        depthViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        depthViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthViewCreateInfo.format = depthFormat;
        depthViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        depthViewCreateInfo.subresourceRange.baseMipLevel = 0;
        depthViewCreateInfo.subresourceRange.levelCount = 1;
        depthViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        depthViewCreateInfo.subresourceRange.layerCount = 1;
        depthStencilView->Create(mDevice, depthViewCreateInfo, mSwapChain.GetImageCount());

        ImageViewVK* colorView = JOJO_NEW(ImageViewVK);
        colorView->imageViews = mSwapChain.imageViews;
        mSwapChain.frameBuffer->colorAttachments.Push(colorView);
        mSwapChain.frameBuffer->SetRawImages(mSwapChain.images);
        mSwapChain.frameBuffer->SetRawImageViews(mSwapChain.imageViews);

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = mSwapChain.extent.width;
        imageInfo.extent.height = mSwapChain.extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = mSwapChain.format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VkSampleCountFlagBits(mSwapChain.frameBuffer->msaaSamples);
        imageInfo.flags = 0; // Optional  
        CreateImage(*colorView->image, imageInfo, mSwapChain.GetImageCount());

        RenderPassVK* renderPass = JOJO_NEW(RenderPassVK);
        mSwapChain.frameBuffer->renderPass = renderPass;

        for (U32 i = 0; i < mSwapChain.GetImageCount(); ++i)
        {
            Vector<VkImageView> attachments;
            if (msaaSamples > 1) {
                //todo
            }
            else
            {
                attachments = {
                    mSwapChain.imageViews[i],
                    depthStencilView->imageViews[i]
                };
            }

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass->renderPass;
            framebufferInfo.attachmentCount = attachments.Size();
            framebufferInfo.pAttachments = attachments.Buffer();
            framebufferInfo.width = mSwapChain.extent.width;
            framebufferInfo.height = mSwapChain.extent.height;
            framebufferInfo.layers = 1;

            VK_CHECK(vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &mSwapChain.frameBuffer->frameBuffers[i]));
        }        
    }

    void RenderAPIVK::CreateSyncObjects(U32 num)
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        mImageAvailableSemaphores.Resize(num);
        mRenderFinishedSemaphores.Resize(num);

        mInFlightFences.Resize(num);
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (U32 i = 0; i < num; ++i) {
            if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {

                LOGERROR("failed to create synchronization objects for a frame!");
            }
        }
    }

    VkFormat RenderAPIVK::FindSupportedFormat(const Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        LOGERROR("failed to find supported format!");
    }
}