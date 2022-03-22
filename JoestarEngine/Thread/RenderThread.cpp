#include "RenderThread.h"
#include "../Graphics/Window.h"
#include "../Graphics/GraphicDefines.h"
#include "../Graphics/Vulkan/RenderAPIVK.h"
#include "../Misc/GlobalConfig.h"
namespace Joestar {
    RenderThread::~RenderThread()
    {
        
    }

    void RenderThread::ThreadFunc()
    {
        if (!bInit) {
            InitContext();
            bInit = true;
            frameIndex = 0;
        }
    }

	void RenderThread::InitContext()
	{
        GlobalConfig* cfg = GetSubsystem<GlobalConfig>();
        U32 api = cfg->GetConfig<U32>(CONFIG_GFX_API);
        if (api == GFX_API_VULKAN)
            mProtocol = (RenderAPIProtocol*)JOJO_NEW(RenderAPIVK);

        Window* window = GetSubsystem<Window>();
        mProtocol->SetWindow(window);
        mProtocol->CreateDevice();
        mProtocol->CreateSwapChain();
        mProtocol->CreateCommandBuffers();
        mProtocol->CreateSyncObjects();
        //glfwSetWindowUserPointer(window);
        //glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //Application* app = Application::GetApplication();
        //GlobalConfig* cfg = app->GetEngineContext()->GetSubsystem<GlobalConfig>();
        //uint32_t width = cfg->GetConfig<uint32_t>(CONFIG_WINDOW_WIDTH);
        //uint32_t height = cfg->GetConfig<uint32_t>(CONFIG_WINDOW_HEIGHT);
        //window = glfwCreateWindow(width, height, "Joestar Engine", nullptr, nullptr);

        //glfwSetWindowUserPointer(window, this);
        //glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //glfwSetCursorPosCallback(window, MouseCallback);
        //glfwSetScrollCallback(window, ScrollCallback);
	}
}