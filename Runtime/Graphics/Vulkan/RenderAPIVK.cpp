#include "RenderAPIVK.h"
#include "../../Container/Vector.h"
#include "../../Container/HashSet.h"
#include "../../Graphics/Descriptor.h"
#include "../../Graphics/SwapChain.h"
#include "../../Graphics/Shader/ShaderReflection.h"
#include "../../Math/MathDefs.h"
#include "../Window.h"
#include <vulkan/vulkan_win32.h>

#define GET_STRUCT_BY_HANDLE_FROM_VECTOR(_VAR, _TYP, _HANDLE, _VEC) \
    if (_HANDLE + 1 > _VEC.Size()) _VEC.Resize(_HANDLE + 1); \
    _VEC[_HANDLE] = JOJO_NEW(_TYP##VK, MEMORY_GFX_STRUCT); \
    _TYP##VK& _VAR =  *(_VEC[_HANDLE]);

#define GET_STRUCT_BY_HANDLE(_VAR, _TYP, _HANDLE) \
    GET_STRUCT_BY_HANDLE_FROM_VECTOR(_VAR, _TYP, _HANDLE, m##_TYP##s);

#define GetFrameCommandBuffer(handle) \
    mCommandBuffers[handle]->GetCommandBuffer(mImageIndex)

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

    U32 RenderAPIVK::FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);
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
            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                indices.transferFamily = i;
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
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        };
        createInfo.enabledExtensionCount = surfaceExtension.Size();
        createInfo.ppEnabledExtensionNames = surfaceExtension.Buffer();
        if (bEnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.Size());
            createInfo.ppEnabledLayerNames = ValidationLayers.Buffer();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }
        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &mInstance))
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
                maxMsaaSamples = GetMaxUsableSampleCount();
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
        HashSet<U32> uniqueQueueFamilies = { mQueueFamilyIndices.graphicsFamily, mQueueFamilyIndices.transferFamily, mQueueFamilyIndices.computeFamily, mQueueFamilyIndices.presentFamily };
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
        VK_CHECK(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice));

        vkGetDeviceQueue(mDevice, mQueueFamilyIndices.graphicsFamily, 0, &mGraphicsQueue);
        vkGetDeviceQueue(mDevice, mQueueFamilyIndices.presentFamily, 0, &mPresentQueue);
        vkGetDeviceQueue(mDevice, mQueueFamilyIndices.computeFamily, 0, &mComputeQueue);
        vkGetDeviceQueue(mDevice, mQueueFamilyIndices.transferFamily, 0, &mTransferQueue);

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
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {

            VkExtent2D actualExtent = {
                window->GetWidth(),
                window->GetHeight()
            };

            actualExtent.width = Min(Max(actualExtent.width, capabilities.minImageExtent.width), capabilities.maxImageExtent.width);
            actualExtent.height = Min(Max(actualExtent.height, capabilities.minImageExtent.height), capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

	void RenderAPIVK::CreateSwapChain()
	{
        //RenderAPIProtocol::CreateSwapChain(ci, num);
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(mPhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);
        //swapChain->width = extent.width;
        //swapChain->height = extent.height;

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
            VkImageViewCreateInfo imageCreateInfo{};
            imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageCreateInfo.image = mSwapChain.images[i];
            imageCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageCreateInfo.format = mSwapChain.format;
            imageCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCreateInfo.subresourceRange.baseMipLevel = 0;
            imageCreateInfo.subresourceRange.levelCount = 1;
            imageCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageCreateInfo.subresourceRange.layerCount = 1;
            VK_CHECK(vkCreateImageView(mDevice, &imageCreateInfo, nullptr, &mSwapChain.imageViews[i]));
        }
	}

    void RenderAPIVK::CreateCommandPool(GPUResourceHandle handle, GPUQueue queue)
    {
        GET_STRUCT_BY_HANDLE(pool, CommandPool, handle);
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        if (GPUQueue::GRAPHICS == queue)
            poolInfo.queueFamilyIndex = mQueueFamilyIndices.graphicsFamily;
        else if (GPUQueue::TRANSFER == queue)
            poolInfo.queueFamilyIndex = mQueueFamilyIndices.transferFamily;
        else if (GPUQueue::COMPUTE == queue)
            poolInfo.queueFamilyIndex = mQueueFamilyIndices.computeFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

        VK_CHECK(vkCreateCommandPool(mDevice, &poolInfo, nullptr, &pool.pool));
    }

    void RenderAPIVK::CreateCommandBuffers(GPUResourceHandle handle, GPUCommandBufferCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(cb, CommandBuffer, handle);

        VkCommandBufferAllocateInfo allocInfo = cb.GetDefaultAllocInfo();
        allocInfo.commandPool = mCommandPools[createInfo.poolHandle]->pool;
        CreateCommandBuffers(cb, allocInfo, createInfo.num);
        if (GPUQueue::GRAPHICS == createInfo.queue)
            cb.SetQueue(mGraphicsQueue);
        else if (GPUQueue::TRANSFER == createInfo.queue)
            cb.SetQueue(mTransferQueue);
        else if (GPUQueue::COMPUTE == createInfo.queue)
            cb.SetQueue(mComputeQueue);
    }

    void RenderAPIVK::CreateCommandBuffers(CommandBufferVK& cb, VkCommandBufferAllocateInfo& allocInfo, U32 num)
    {
        allocInfo.commandBufferCount = num;
        cb.Create(mDevice, allocInfo, num);
    }

    CommandBufferVK RenderAPIVK::GetTempCommandBuffer()
    {
        CommandBufferVK cb;
        VkCommandBufferAllocateInfo allocInfo = cb.GetDefaultAllocInfo();
        allocInfo.commandPool = mCommandPools[0]->pool;
        CreateCommandBuffers(cb, allocInfo, 1);      
        cb.SetTemp(true);
        cb.SetQueue(mGraphicsQueue);
        return std::move(cb);
    }

    //void RenderAPIVK::CreateMainCommandBuffers(U32 num)
    //{
    //    CreateCommandPool();

    //    GPUCommandBufferCreateInfo ci;
    //    return CreateCommandBuffers(0, ci, num);
    //}

    void RenderAPIVK::CreateImage(GPUResourceHandle handle, GPUImageCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(image, Image, handle);
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = GetImageTypeVK(createInfo.type);
        imageInfo.extent.width = createInfo.width;
        imageInfo.extent.height = createInfo.height;
        imageInfo.extent.depth = createInfo.depth;
        imageInfo.mipLevels = createInfo.mipLevels;
        imageInfo.arrayLayers = createInfo.layer;
        imageInfo.format = GetImageFormatVK(createInfo.format);
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = GetImageUsageBitsVK(createInfo.usage);;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VkSampleCountFlagBits(createInfo.samples);
        if (createInfo.layer == 6)
            imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT; // Optional  
        else
            imageInfo.flags = 0; // Optional

        GPUMemory& mem = mMemories[createInfo.memHandle];
        StagingBufferVK* stagingBuffer = JOJO_NEW(StagingBufferVK, MEMORY_GFX_STRUCT);
        stagingBuffer->count = 1;
        stagingBuffer->SetDevice(mDevice, mPhysicalDevice);
        stagingBuffer->SetSize(mem.size);
        stagingBuffer->Create(mem.data);
        image.SetStagingBuffer(stagingBuffer);

        CreateImage(image, imageInfo, createInfo.num);
    }

    void RenderAPIVK::CreateImage(ImageVK& image, VkImageCreateInfo& imageInfo, U32 num)
    {
        image.Create(mDevice, imageInfo, num);

        U32 memoryTypeIdx = FindMemoryType(image.memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        image.AllocMemory(mDevice, memoryTypeIdx);
    }


    void RenderAPIVK::CreateImageView(GPUResourceHandle handle, GPUImageViewCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(imageView, ImageView, handle);
        ImageVK& image = *mImages[createInfo.imageHandle];
        imageView.SetImage(&image);
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType = GetImageViewTypeVK(createInfo.type);
        viewInfo.format = GetImageFormatVK(createInfo.format);
        viewInfo.subresourceRange.aspectMask = GetImageAspectBitsVK(createInfo.aspectBits);
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = createInfo.mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = createInfo.baseLayer;
        viewInfo.subresourceRange.layerCount = createInfo.layer;
        imageView.Create(mDevice, viewInfo, createInfo.num);
    }

    void RenderAPIVK::CreateFrameBuffers(GPUResourceHandle handle, GPUFrameBufferCreateInfo& createInfo)
    {

    }

    void RenderAPIVK::CreateRenderPass(RenderPassVK* rp, GPURenderPassCreateInfo& createInfo)
    {
        bool bMSAA = createInfo.msaaSamples > 1;
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = GetImageFormatVK(createInfo.colorFormat);
        colorAttachment.samples = VkSampleCountFlagBits(createInfo.msaaSamples);
        colorAttachment.loadOp = VkAttachmentLoadOp(createInfo.colorLoadOp);
        colorAttachment.storeOp = VkAttachmentStoreOp(createInfo.colorStoreOp);
        colorAttachment.stencilLoadOp = VkAttachmentLoadOp(createInfo.stencilLoadOp);
        colorAttachment.stencilStoreOp = VkAttachmentStoreOp(createInfo.stencilStoreOp);
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = bMSAA ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_GENERAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = GetImageFormatVK(createInfo.depthStencilFormat);
        depthAttachment.samples = VkSampleCountFlagBits(createInfo.msaaSamples);
        depthAttachment.loadOp = VkAttachmentLoadOp(createInfo.depthLoadOp);
        depthAttachment.storeOp = VkAttachmentStoreOp(createInfo.depthStoreOp);
        depthAttachment.stencilLoadOp = VkAttachmentLoadOp(createInfo.stencilLoadOp);
        depthAttachment.stencilStoreOp = VkAttachmentStoreOp(createInfo.stencilStoreOp);
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkRenderPassCreateInfo renderPassInfo{};
        if (bMSAA)
        {
            VkAttachmentDescription colorAttachmentResolve{};
            colorAttachmentResolve.format = mSwapChain.format;
            colorAttachmentResolve.samples = VkSampleCountFlagBits(mSwapChain.frameBuffer->msaaSamples);
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentResolveRef{};
            colorAttachmentResolveRef.attachment = 2;
            colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            subpass.pDepthStencilAttachment = &depthAttachmentRef;
            subpass.pResolveAttachments = &colorAttachmentResolveRef;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            Vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = attachments.Size();
            renderPassInfo.pAttachments = attachments.Buffer();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            VK_CHECK(vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &(rp->renderPass)));
        }
        else
        {
            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            subpass.pDepthStencilAttachment = &depthAttachmentRef;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            Vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = attachments.Size();
            renderPassInfo.pAttachments = attachments.Buffer();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            VK_CHECK(vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &(rp->renderPass)));
        }

    }

    void RenderAPIVK::CreateTexture(GPUResourceHandle handle, GPUTextureCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE_FROM_VECTOR(texture, Texture, handle, mTextures);
        texture.Create(mImageViews[createInfo.imageViewHandle], mSamplers[createInfo.samplerHandle]);
    }

    void RenderAPIVK::CreateBackBuffers(GPUFrameBufferCreateInfo& createInfo)
    {
        if (mSwapChain.frameBuffer)
            return;
        mSwapChain.frameBuffer = JOJO_NEW(FrameBufferVK);
        if (mFrameBuffers.Empty())
            mFrameBuffers.Resize(1);
        mFrameBuffers[0] = mSwapChain.frameBuffer;

        mSwapChain.frameBuffer->frameBuffers.Resize(mSwapChain.GetImageCount());
        VkFormat depthFormat = FindSupportedFormat(
            { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );

        ImageVK* depthImage = JOJO_NEW(ImageVK, MEMORY_GFX_STRUCT);
        VkImageCreateInfo depthImgCreateInfo{};
        depthImgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        depthImgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        depthImgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        depthImgCreateInfo.extent.width = mSwapChain.extent.width;
        depthImgCreateInfo.extent.height = mSwapChain.extent.height;
        depthImgCreateInfo.extent.depth = 1;
        depthImgCreateInfo.arrayLayers = 1;
        depthImgCreateInfo.format = depthFormat;
        depthImgCreateInfo.mipLevels = 1;
        depthImgCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImgCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImgCreateInfo.samples = (VkSampleCountFlagBits)createInfo.msaaSamples;
        CreateImage(*depthImage, depthImgCreateInfo, mSwapChain.GetImageCount());

        ImageViewVK* depthStencilView = JOJO_NEW(ImageViewVK);
        depthStencilView->SetImage(depthImage);
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
        CommandBufferVK cb = GetTempCommandBuffer();
        cb.Begin();
        depthImage->TransitionImageLayout(cb.GetCommandBuffer(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        cb.End();
        cb.Submit();
        ImageViewVK* colorView = JOJO_NEW(ImageViewVK);
        colorView->SetRawImageViews(mSwapChain.imageViews);
        mSwapChain.frameBuffer->colorAttachments.Push(colorView);
        mSwapChain.frameBuffer->SetRawImages(mSwapChain.images);
        mSwapChain.frameBuffer->SetRawImageViews(mSwapChain.imageViews);

        //VkImageCreateInfo imageInfo{};
        //imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        //imageInfo.imageType = VK_IMAGE_TYPE_2D;
        //imageInfo.extent.width = mSwapChain.extent.width;
        //imageInfo.extent.height = mSwapChain.extent.height;
        //imageInfo.extent.depth = 1;
        //imageInfo.mipLevels = 1;
        //imageInfo.arrayLayers = 1;
        //imageInfo.format = mSwapChain.format;
        //imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        //imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        //imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //imageInfo.samples = VkSampleCountFlagBits(mSwapChain.frameBuffer->msaaSamples);
        //imageInfo.flags = 0; // Optional  
        //CreateImage(*colorView->image, imageInfo, mSwapChain.GetImageCount());

        //VkImageViewCreateInfo colorViewCreateInfo{};
        //colorViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        //colorViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        //colorViewCreateInfo.format = mSwapChain.format;
        //colorViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        //colorViewCreateInfo.subresourceRange.baseMipLevel = 0;
        //colorViewCreateInfo.subresourceRange.levelCount = 1;
        //colorViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        //colorViewCreateInfo.subresourceRange.layerCount = 1;
        //colorView->Create(mDevice, colorViewCreateInfo, mSwapChain.GetImageCount());
        //cb = GetTempCommandBuffer();
        //colorView->TransitionImageLayout(cb, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

        RenderPassVK* renderPass = JOJO_NEW(RenderPassVK);
        GPURenderPassCreateInfo rpInfo;
        CreateRenderPass(renderPass, rpInfo);

        mSwapChain.frameBuffer->renderPass = renderPass;

        for (U32 i = 0; i < mSwapChain.GetImageCount(); ++i)
        {
            Vector<VkImageView> attachments;
            if (createInfo.msaaSamples > 1) {
                //todo
            }
            else
            {
                attachments = {
                    mSwapChain.imageViews[i],
                    depthStencilView->GetImageView(i)
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
            VK_CHECK(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]));
            VK_CHECK(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]));
            VK_CHECK(vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]));
        }
    }

    void RenderAPIVK::CreateIndexBuffer(GPUResourceHandle handle, GPUIndexBufferCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(indexBuffer, IndexBuffer, handle);
        indexBuffer.SetDevice(mDevice, mPhysicalDevice);
        GPUMemory& mem = mMemories[createInfo.memoryHandle];

        //StagingBufferVK stagingBuffer;
        //stagingBuffer.SetDevice(mDevice, mPhysicalDevice);
        //stagingBuffer.size = mem.size;
        //stagingBuffer.Create(mem.data);

        indexBuffer.Create(createInfo.indexSize, createInfo.indexCount);
        indexBuffer.CreateStagingBuffer(mem.size, mem.data);
        //CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer.GetBuffer(), indexBuffer.size);
    }

    void RenderAPIVK::CreateVertexBuffer(GPUResourceHandle handle, GPUVertexBufferCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(vertexBuffer, VertexBuffer, handle);
        vertexBuffer.SetDevice(mDevice, mPhysicalDevice);
        GPUMemory& mem = mMemories[createInfo.memoryHandle];

        vertexBuffer.Create(createInfo.vertexSize, createInfo.vertexCount, createInfo.elements);
        vertexBuffer.CreateStagingBuffer(mem.size, mem.data);
    }

    void RenderAPIVK::CreateUniformBuffer(GPUResourceHandle handle, GPUUniformBufferCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(uniformBuffer, UniformBuffer, handle);
        uniformBuffer.SetDevice(mDevice, mPhysicalDevice);
        uniformBuffer.count = mSwapChain.GetImageCount();
        uniformBuffer.size = createInfo.size;
        uniformBuffer.Create(uniformBuffer.size);
        uniformBuffer.CreateStagingBuffer(uniformBuffer.size, nullptr);
    }

    void RenderAPIVK::SetUniformBuffer(GPUResourceHandle handle, U8* data, U32 size)
    {
        mUniformBuffers[handle]->SetFrame(mImageIndex);
        mUniformBuffers[handle]->UpdateStagingBuffer(size, data);

        //StagingBufferVK stagingBuffer;
        //stagingBuffer.SetDevice(mDevice, mPhysicalDevice);
        //stagingBuffer.size = size;
        //stagingBuffer.Create(data);

        //CopyBuffer(stagingBuffer.GetBuffer(), mUniformBuffers[handle]->GetBuffer(), size);
    }

    void RenderAPIVK::CreateRenderPass(GPUResourceHandle handle, GPURenderPassCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE_FROM_VECTOR(renderPass, RenderPass, handle, mRenderPasses);
        CreateRenderPass(&renderPass, createInfo);
    }

    void RenderAPIVK::CreatePipelineLayout(GPUResourceHandle handle, GPUPipelineLayoutCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE_FROM_VECTOR(layout, PipelineLayout, handle, mPipelineLayouts);
        PODVector<VkDescriptorSetLayout> setLayouts;
        setLayouts.Resize(createInfo.numLayouts);
        for (U32 i = 0; i < createInfo.numLayouts; ++i)
        {
            setLayouts[i] = mDescriptorSetLayouts[createInfo.setLayoutHandles[i]]->setLayout;
        }
        layout.Create(mDevice, setLayouts);
    }

    void RenderAPIVK::CreateDescriptorSetLayout(GPUResourceHandle handle, PODVector<GPUDescriptorSetLayoutBinding>& bindings)
    {
        GET_STRUCT_BY_HANDLE_FROM_VECTOR(setLayout, DescriptorSetLayout, handle, mDescriptorSetLayouts);
        setLayout.Create(mDevice, bindings);
    }

    void RenderAPIVK::CreateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE_FROM_VECTOR(sets, DescriptorSets, handle, mDescriptorSets);
        PODVector<VkDescriptorSetLayout> layouts;
        layouts.Resize(mSwapChain.GetImageCount());
        for (U32 i = 0; i < mSwapChain.GetImageCount(); ++i)
        {
            layouts[i] = mDescriptorSetLayouts[createInfo.layoutHandle]->setLayout;
        }
        sets.Create(mDevice, mDescriptorPool, layouts);
    }

    void RenderAPIVK::UpdateDescriptorSets(GPUResourceHandle handle, GPUDescriptorSetsUpdateInfo& updateInfo)
    {
        Vector<VkWriteDescriptorSet> descriptorWrites;
        descriptorWrites.Resize(updateInfo.num);
        U32 idx = mFrameIndex % 3;
        for (U32 i = 0; i < updateInfo.num; ++i) {
            descriptorWrites[i] = {};
            descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            auto& entry = updateInfo.updateSets[i];
            descriptorWrites[i].dstSet = mDescriptorSets[entry.setHandle]->GetDescriptorSets(idx);
            descriptorWrites[i].dstBinding = entry.binding;
            descriptorWrites[i].dstArrayElement = 0;
            descriptorWrites[i].descriptorType = GetDescriptorTypeVK(entry.type);
            descriptorWrites[i].descriptorCount = 1;
            if (entry.type == DescriptorType::COMBINED_IMAGE_SAMPLER)
            {
                VkDescriptorImageInfo imageInfo;
                descriptorWrites[i].pImageInfo = &mTextures[entry.textureHandle]->GetDescriptorImageInfo(idx);
            }
            else
            {
                descriptorWrites[i].pBufferInfo = &mUniformBuffers[entry.uniformHandle]->GetDescriptorBufferInfo(idx);
            }
            
            vkUpdateDescriptorSets(mDevice, descriptorWrites.Size(), descriptorWrites.Buffer(), 0, nullptr);
        }
    }

    void RenderAPIVK::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        CommandBufferVK cb = GetTempCommandBuffer();
        cb.Begin();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(cb.GetCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);
        
        cb.End();
        cb.Submit();
    }

    void RenderAPIVK::CreateDescriptorPool(U32 num) {
        //already exist
        if (mDescriptorPool != VK_NULL_HANDLE) return;
        Vector<VkDescriptorPoolSize> poolSizes;
        //创建一个足够大的池子，后续有需要还要补扩容逻辑
        poolSizes.Push({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER , maxUniformBuffers * num });
        poolSizes.Push({ VK_DESCRIPTOR_TYPE_SAMPLER , maxBindings * num });
        poolSizes.Push({ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE , maxBindings * num });
        poolSizes.Push({ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE , maxBindings * num });
        poolSizes.Push({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER , maxUniformBuffers * num });
        poolSizes.Push({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER , maxBindings * num });
        poolSizes.Push({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC , maxBindings * num });
        poolSizes.Push({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC , maxBindings * num });
        poolSizes.Push({ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT , maxBindings * num });

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.Size();
        poolInfo.pPoolSizes = poolSizes.Buffer();
        //先给大点？应该没啥问题？
        poolInfo.maxSets = maxUniformBuffers * 2;

        VK_CHECK(vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool));
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


    void RenderAPIVK::CreateGraphicsPipelineState(GPUResourceHandle handle, GPUGraphicsPipelineStateCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(pso, GraphicsPipelineState, handle);
        
        pso.CreateIAState();
        //这里可能实际创建出来的小于window设定的size。
        if (createInfo.viewport.rect.width > mSwapChain.extent.width)
        {
            createInfo.viewport.SetSize(mSwapChain.extent.width, mSwapChain.extent.height);
        }
        pso.CreateViewportState(createInfo.viewport);
        pso.CreateRasterizationState(*(mRasterizationStates[createInfo.rasterizationStateHandle]));
        pso.CreateMultiSampleState(*(mMultiSampleStates[createInfo.multiSampleStateHandle]));
        pso.CreateDepthStencilState(*(mDepthStencilStates[createInfo.depthStencilStateHandle]));
        pso.CreateColorBlendState(*(mColorBlendStates[createInfo.colorBlendStateHandle]));

        GPUShaderProgramCreateInfo& program = *(mShaderPrograms[createInfo.shaderProramHandle]);
        PODVector<ShaderVK*> shaders;
        for (U32 i = 0; i < program.numStages; ++i)
        {
            shaders.Push(mShaders[program.shaderHandles[i]]);
        }
        pso.CreateShaderStages(shaders);

        pso.CreateVertexInputInfo(createInfo.inputBindings, createInfo.inputAttributes);

        pso.SetRenderPass(mRenderPasses[createInfo.renderPassHandle]);
        pso.SetPipelineLayout(mPipelineLayouts[createInfo.pipelineLayoutHandle]->layout);
        pso.Create(mDevice);
    }

    void RenderAPIVK::CreateComputePipelineState(GPUResourceHandle handle, GPUComputePipelineStateCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(pso, ComputePipelineState, handle);
        pso.Create(createInfo);
    }


    void RenderAPIVK::CreateShader(GPUResourceHandle handle, GPUShaderCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(shader, Shader, handle);
        shader.Create(mDevice, createInfo);
    }

    void RenderAPIVK::CreateSampler(GPUResourceHandle handle, GPUSamplerCreateInfo& createInfo)
    {
        GET_STRUCT_BY_HANDLE(sampler, Sampler, handle);
        sampler.Create(mDevice, createInfo);
    }

    ///CommandBuffer Commands
    void RenderAPIVK::CBBegin(GPUResourceHandle handle)
    {
        mCommandBuffers[handle]->Begin(mImageIndex);
    }
    void RenderAPIVK::CBEnd(GPUResourceHandle handle)
    {
        mCommandBuffers[handle]->End(mImageIndex);
    }
    void RenderAPIVK::CBBeginRenderPass(GPUResourceHandle handle, RenderPassBeginInfo& beginInfo)
    {
        RenderPassVK* pass = mRenderPasses[beginInfo.passHandle];
        FrameBufferVK* fb = mFrameBuffers[beginInfo.fbHandle];
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pass->renderPass;
        renderPassInfo.framebuffer = fb->GetFrameBuffer(mImageIndex);
        renderPassInfo.renderArea.offset = { (I32)beginInfo.renderArea.x, (I32)beginInfo.renderArea.y };
        renderPassInfo.renderArea.extent = { (U32)Min(beginInfo.renderArea.width, mSwapChain.extent.width), 
            (U32)Min(beginInfo.renderArea.height, mSwapChain.extent.height) };
        Vector<VkClearValue> clearValues;
        if (beginInfo.numClearValues > 0) {
            clearValues.Resize(beginInfo.numClearValues);
            for (U32 i = 0; i < beginInfo.numClearValues; ++i)
            {
                //深度默认放最后
                if (i == beginInfo.numClearValues - 1)
                {
                    clearValues[i].depthStencil = { beginInfo.clearValues[i].depthStencil.depth, beginInfo.clearValues[i].depthStencil.stencil };
                }
                else
                {
                    clearValues[i].color = { beginInfo.clearValues[i].color.f32[0], beginInfo.clearValues[i].color.f32[1], beginInfo.clearValues[i].color.f32[2], beginInfo.clearValues[i].color.f32[3] };
                }
            }
            renderPassInfo.clearValueCount = clearValues.Size();
            renderPassInfo.pClearValues = clearValues.Buffer();
        }
        vkCmdBeginRenderPass(GetFrameCommandBuffer(handle), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
    void RenderAPIVK::CBEndRenderPass(GPUResourceHandle handle, GPUResourceHandle)
    {
        vkCmdEndRenderPass(GetFrameCommandBuffer(handle));
    }
    void RenderAPIVK::CBBindGraphicsPipeline(GPUResourceHandle handle, GPUResourceHandle h)
    {
        vkCmdBindPipeline(GetFrameCommandBuffer(handle), VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipelineStates[h]->GetPipeline());
    }

    void RenderAPIVK::CBBindComputePipeline(GPUResourceHandle handle, GPUResourceHandle h)
    {
        vkCmdBindPipeline(GetFrameCommandBuffer(handle), VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipelineStates[h]->GetPipeline());

    }
    void RenderAPIVK::CBBindIndexBuffer(GPUResourceHandle handle, GPUResourceHandle h)
    {
        vkCmdBindIndexBuffer(GetFrameCommandBuffer(handle), mIndexBuffers[h]->GetBuffer(), 0, mIndexBuffers[h]->GetIndexType());
    }
    void RenderAPIVK::CBBindVertexBuffer(GPUResourceHandle handle, GPUResourceHandle h, U32 binding)
    {
        VkBuffer buffer = mVertexBuffers[h]->GetBuffer();
        VkDeviceSize offsets[] = { 0, 0 };
        vkCmdBindVertexBuffers(GetFrameCommandBuffer(handle), 0, 1, &buffer, offsets);
    }
    void RenderAPIVK::CBBindDescriptorSets(GPUResourceHandle handle, GPUResourceHandle layoutHandle, GPUResourceHandle setsHandle, U32 set)
    {
        DescriptorSetsVK* sets = mDescriptorSets[setsHandle];
        VkDescriptorSet vkSets = sets->GetDescriptorSets(mImageIndex);
        PipelineLayoutVK& layout = *mPipelineLayouts[layoutHandle];
        vkCmdBindDescriptorSets(GetFrameCommandBuffer(handle), layout.GetBindPoint(), layout.layout, set,
            1, &vkSets, 0, nullptr);
    }
    void RenderAPIVK::CBPushConstants(GPUResourceHandle handle, GPUResourceHandle)
    {
        //todo
    }
    void RenderAPIVK::CBDraw(GPUResourceHandle handle, U32 count)
    {
        vkCmdDraw(GetFrameCommandBuffer(handle), count, 1, 0, 0);
    }
    void RenderAPIVK::CBDrawIndexed(GPUResourceHandle handle, U32 count, U32 indexStart, U32 vertStart)
    {
        vkCmdDrawIndexed(GetFrameCommandBuffer(handle), count, 1, indexStart, vertStart, 0);
    }
    void RenderAPIVK::CBCopyBuffer(GPUResourceHandle handle, CopyBufferType type, GPUResourceHandle bufferHandle)
    {
        if (CopyBufferType::VB == type)
        {
            mVertexBuffers[bufferHandle]->CopyBuffer(GetFrameCommandBuffer(handle));
        }
        else if (CopyBufferType::IB == type)
        {
            mIndexBuffers[bufferHandle]->CopyBuffer(GetFrameCommandBuffer(handle));
        }
        else if (CopyBufferType::UB == type)
        {
            mUniformBuffers[bufferHandle]->CopyBuffer(GetFrameCommandBuffer(handle));
        }
    }
    void RenderAPIVK::CBCopyBufferToImage(GPUResourceHandle handle, GPUResourceHandle imageHandle, ImageLayout layout)
    {
        mImages[imageHandle]->CopyBufferToImage(GetFrameCommandBuffer(handle), layout);
    }

    void RenderAPIVK::CBTransitionImageLayout(GPUResourceHandle handle, GPUResourceHandle imageHandle, ImageLayout oldLayout, ImageLayout newLayout, U32 aspectFlags)
    {
        mImages[imageHandle]->TransitionImageLayout(GetFrameCommandBuffer(handle), oldLayout, newLayout, aspectFlags);
    }
    void RenderAPIVK::CBSubmit(GPUResourceHandle handle)
    {
        mCommandBuffers[handle]->SetIndex(mImageIndex);
        mCommandBuffers[handle]->Submit();
    }

    void RenderAPIVK::BeginFrame(U32 frameIndex)
    {
        RenderAPIProtocol::BeginFrame(frameIndex);
        //第一帧还没创建，先跳过
        if (mInFlightFences.Empty())
        {
            bSync = false;
            return;
        }
        vkWaitForFences(mDevice, 1, &mInFlightFences[frameIndex], VK_TRUE, U64_MAX);
        vkResetFences(mDevice, 1, &mInFlightFences[frameIndex]);
        VkResult result = vkAcquireNextImageKHR(mDevice, mSwapChain.swapChain, UINT64_MAX, mImageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, &mImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || bResized) {
            bResized = false;
            //todo 暂时不搞这个
            //RecreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            LOGERROR("Failed to acquire swap chain image!");
        }
        bSync = true;
    }
    void RenderAPIVK::EndFrame(U32 frameIndex)
    {
        RenderAPIProtocol::EndFrame(frameIndex);
    }

    void RenderAPIVK::QueueSubmit(GPUResourceHandle handle)
    {
        VkCommandBuffer& cb = GetFrameCommandBuffer(handle);
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        if (bSync)
        {
            VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mFrameIndex] };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
        }

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cb;

        VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mFrameIndex] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VK_CHECK(vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, bSync ? mInFlightFences[mFrameIndex] : VK_NULL_HANDLE));
    }

    void RenderAPIVK::Present()
    {
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mFrameIndex] };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { mSwapChain.swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &mImageIndex;
        presentInfo.pResults = nullptr; // Optional
        VK_CHECK(vkQueuePresentKHR(mPresentQueue, &presentInfo));
    }
}