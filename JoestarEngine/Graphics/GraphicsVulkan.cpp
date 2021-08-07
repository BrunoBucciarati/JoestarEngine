#include "GraphicsVulkan.h"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
void processInputVulkan(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //float cameraSpeed = 0.05f; // adjust accordingly
    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //    cam->ProcessKeyboard(FORWARD, deltaTime);
    //if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //    cam->ProcessKeyboard(BACKWARD, deltaTime);
    //if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //    cam->ProcessKeyboard(LEFT, deltaTime);
    //if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //    cam->ProcessKeyboard(RIGHT, deltaTime);
}

bool GraphicsVulkan::CheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

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

void GraphicsVulkan::Init()
{
    InitVulkan();
}

void GraphicsVulkan::InitVulkan()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    uint32_t width = GetGlobalConfig()->GetConfig<uint32_t>("WINDOW_WIDTH");
    uint32_t height = GetGlobalConfig()->GetConfig<uint32_t>("WINDOW_HEIGHT");
    window = glfwCreateWindow(width, height, "Joestar Engine", nullptr, nullptr);

    CreateInstance();


    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInputVulkan(window);

        glfwPollEvents();
        MainLoop();
    }

    Cleanup();
}

void GraphicsVulkan::CreateInstance() {
    if (enableValidationLayers && !CheckValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
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

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void GraphicsVulkan::Cleanup() {
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);

    glfwTerminate();
}

void GraphicsVulkan::DrawTriangle() {

}