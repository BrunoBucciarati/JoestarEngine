#include "RenderThread.h"
#include "../Graphics/Window.h"
#include "../Graphics/GraphicDefines.h"
#include "../Graphics/GFXCommandList.h"
#include "../Graphics/Vulkan/RenderAPIVK.h"
#include "../Misc/GlobalConfig.h"
#include "../Graphics/GPUCreateInfos.h"
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
            while (!mCmdList[idx]->readFlag) {
                //wait for 0.01 sec
                Sleep(10.F);
            }

            GFXCommand command;
            while (mCmdList[idx]->ReadCommand(command))
            {
                ExecuteGFXCommand((U32)command, mCmdList[idx]);
            }
            //已经处理完这波指令了，清掉
            mCmdList[idx]->Clear();
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
                break;
            }
            case GFXCommand::CreateFrameBuffer:
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
                break;
            }
            case GFXCommand::CreateImage:
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUImageCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);

                mProtocol->CreateImage(handle, createInfo);
                break;
            }
            case GFXCommand::CreateImageView:
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUImageViewCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateImageView(handle, createInfo);
                break;
            }
            case GFXCommand::CreateSwapChain:
            {
                GPUSwapChainCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateSwapChain(createInfo);
                break;
            }
            case GFXCommand::CreateSyncObjects:
            {
                U32 num;
                cmdList->ReadBuffer<U32>(num);
                mProtocol->CreateSyncObjects(num);
                break;
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
	}
}