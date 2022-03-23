#include "RenderThread.h"
#include "../Graphics/Window.h"
#include "../Graphics/GraphicDefines.h"
#include "../Graphics/GFXCommandList.h"
#include "../Graphics/Vulkan/RenderAPIVK.h"
#include "../Misc/GlobalConfig.h"
namespace Joestar {
#define MAX_FRAMES_IN_FLIGHT 3
    RenderThread::~RenderThread()
    {
        
    }

    void RenderThread::SetGFXCommandList(GFXCommandList** list)
    {
        for (int i = 0; i < MAX_CMDLISTS_IN_FLIGHT; ++i)
        {
            mCmdList[i] = list[i];
        }
    }

    void RenderThread::ThreadFunc()
    {
        if (!bInit) {
            InitContext();
            bInit = true;
            frameIndex = 0;
        }

        while (!bExit) {
            if (!mCmdList)
                continue;
            U32 idx = frameIndex % MAX_CMDLISTS_IN_FLIGHT;
            //always dispatch compute first, then draw
            while (!mCmdList[idx]->readFlag) {
                //wait for 0.1 sec
                Sleep(10.F);
            }

            GFXCommand command;
            while (mCmdList[idx]->ReadCommand(command))
            {
                ExecuteGFXCommand((U32)command, mCmdList[idx]);
            }
            mCmdList[idx]->readFlag = false;
            ++frameIndex;
        }
    }

    void RenderThread::ExecuteGFXCommand(U32 command, GFXCommandList* cmdList)
    {
        switch (GFXCommand(command))
        {
            case GFXCommand::CreateCommandBuffer:
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);
                if (0 == handle)
                {
                    mProtocol->CreateMainCommandBuffers(MAX_CMDLISTS_IN_FLIGHT);
                    break;
                }
                GPUResourceCreateInfo createInfo;
                cmdList->ReadBuffer<GPUResourceCreateInfo>(createInfo);
                mProtocol->CreateCommandBuffers(handle, createInfo);
            }
            default: break;
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
        //mProtocol->CreateMainCommandBuffers(MAX_FRAMES_IN_FLIGHT);
        mProtocol->CreateSyncObjects(info, MAX_FRAMES_IN_FLIGHT);
	}
}