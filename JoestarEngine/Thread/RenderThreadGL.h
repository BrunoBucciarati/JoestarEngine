#pragma once
#include <glad/glad.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "RenderThread.h"
namespace Joestar {
	class RenderThreadGL : public RenderThread {
		//void InitRenderContext();
		//bool CheckValidationLayerSupport();
		//void CreateInstance();
		//void Cleanup();

	private:
		GLFWwindow* window;
		VkInstance instance;
	};
}