#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.h>
#include <vector>
#include "GlobalConfig.h"
#include <iostream>
class VulkanApplication {
public:
    void Run();

private:
    GLFWwindow* window;
    VkInstance instance;
    void InitVulkan();

    void MainLoop();

    void Cleanup();

    void CreateInstance();

    bool CheckValidationLayerSupport();
};