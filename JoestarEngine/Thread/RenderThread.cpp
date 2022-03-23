#include "RenderThread.h"
#include "../Graphics/Window.h"
#include "../Graphics/GraphicDefines.h"
#include "../Graphics/Vulkan/RenderAPIVK.h"
#include "../Misc/GlobalConfig.h"
namespace Joestar {
#define MAX_FRAMES_IN_FLIGHT 3
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

        GPUResourceCreateInfo info{};
        mProtocol->CreateSwapChain(info, MAX_FRAMES_IN_FLIGHT);
        mProtocol->CreateMainCommandBuffers(MAX_FRAMES_IN_FLIGHT);
        mProtocol->CreateSyncObjects(info, MAX_FRAMES_IN_FLIGHT);
	}
}