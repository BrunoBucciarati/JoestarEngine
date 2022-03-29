#include "RenderThread.h"
#include "../Graphics/Window.h"
#include "../Graphics/GraphicDefines.h"
#include "../Graphics/GFXCommandList.h"
#include "../Graphics/Vulkan/RenderAPIVK.h"
#include "../Misc/GlobalConfig.h"
#include "../Graphics/GPUCreateInfos.h"
#include "../IO/Log.h"
namespace Joestar {
#define MAX_FRAMES_IN_FLIGHT 3
#define DEBUG_CMD 1
#if DEBUG_CMD
#define CASECMD(CMD_TYPE) \
    case CMD_TYPE: \
    LOGWARN("Frame:%d CMD:%s\n",frameIndex, #CMD_TYPE);
#else
#define CASECMD(CMD_TYPE) \
    case CMD_TYPE:
#endif

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
            mProtocol->SetFrame(idx);
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
            CASECMD(GFXCommand::CreateCommandBuffer)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);
                if (0 == handle)
                {
                    mProtocol->CreateMainCommandBuffers(MAX_CMDLISTS_IN_FLIGHT);
                    break;
                }
                GPUCommandBufferCreateInfo createInfo;
                cmdList->ReadBuffer<GPUCommandBufferCreateInfo>(createInfo);
                mProtocol->CreateCommandBuffers(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateFrameBuffer)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUFrameBufferCreateInfo createInfo;
                cmdList->ReadBuffer<GPUFrameBufferCreateInfo>(createInfo);
                if (0 == handle)
                {
                    mProtocol->CreateBackBuffers(createInfo);
                    break;
                }
                mProtocol->CreateFrameBuffers(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateImage)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUImageCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);

                mProtocol->CreateImage(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateImageView)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUImageViewCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateImageView(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateSwapChain)
            {
                GPUSwapChainCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateSwapChain(createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateSyncObjects)
            {
                U32 num;
                cmdList->ReadBuffer<U32>(num);
                mProtocol->CreateSyncObjects(num);
                break;
            }
            CASECMD(GFXCommand::CreateIndexBuffer)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUIndexBufferCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateIndexBuffer(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateVertexBuffer)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUVertexBufferCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);

                createInfo.elements.Reserve(createInfo.elementCount);
                for (U32 i = 0; i < createInfo.elementCount; ++i)
                {
                    VertexElement element;
                    cmdList->ReadBuffer(element);
                    createInfo.elements.Push(element);
                }
                mProtocol->CreateVertexBuffer(handle, createInfo);

                break;
            }
            CASECMD(GFXCommand::CreateUniformBuffer)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUUniformBufferCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateUniformBuffer(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateRenderPass)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPURenderPassCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateRenderPass(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateMemory)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                U32 size;
                cmdList->ReadBuffer(size);

                U8* data = JOJO_NEW_ARRAY(U8, size, MEMORY_GFX_MEMORY);
                cmdList->ReadBufferPtr(data, size);

                mProtocol->CreateMemory(handle, size, data);
                break;
            }
            CASECMD(GFXCommand::CreateColorBlendState)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUColorBlendStateCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                createInfo.attachments.Resize(createInfo.numAttachments);
                for (U32 i = 0; i < createInfo.numAttachments; ++i)
                {
                    cmdList->ReadBuffer(createInfo.attachments[i]);

                }
                mProtocol->CreateColorBlendState(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateDepthStencilState)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUDepthStencilStateCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateDepthStencilState(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateRasterizationState)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPURasterizationStateCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateRasterizationState(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateMultiSampleState)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUMultiSampleStateCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateMultiSampleState(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateGraphicsPipelineState)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUGraphicsPipelineStateCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);

                createInfo.inputBindings.Clear();
                createInfo.inputBindings.Reserve(createInfo.numInputBindings);
                for (U32 i = 0; i < createInfo.numInputBindings; ++i)
                {
                    InputBinding binding;
                    cmdList->ReadBuffer(binding);
                    createInfo.inputBindings.Push(binding);
                }

                createInfo.inputAttributes.Clear();
                createInfo.inputAttributes.Reserve(createInfo.numInputAttributes);
                for (U32 i = 0; i < createInfo.numInputAttributes; ++i)
                {
                    InputAttribute attr;
                    cmdList->ReadBuffer(attr);
                    createInfo.inputAttributes.Push(attr);
                }
                mProtocol->CreateGraphicsPipelineState(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateComputePipelineState)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUComputePipelineStateCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateComputePipelineState(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateDescriptorPool)
            {
                U32 num;
                cmdList->ReadBuffer(num);
                mProtocol->CreateDescriptorPool(num);
                break;
            }
            CASECMD(GFXCommand::CreateShader)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUShaderCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateShader(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateShaderProgram)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                GPUShaderProgramCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                createInfo.shaderHandles.Reserve(createInfo.numStages);
                createInfo.setLayoutHandles.Resize(createInfo.numSetLayouts);
                for (U32 i = 0; i < createInfo.numStages; ++i)
                {
                    GPUResourceHandle shaderHandle;
                    cmdList->ReadBuffer<GPUResourceHandle>(shaderHandle);
                    createInfo.shaderHandles.Push(shaderHandle);
                }
                for (U32 i = 0; i < createInfo.numSetLayouts; ++i)
                {
                    GPUResourceHandle layoutHandle;
                    cmdList->ReadBuffer<GPUResourceHandle>(layoutHandle);
                    createInfo.setLayoutHandles.Push(layoutHandle);
                }

                mProtocol->CreateShaderProgram(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::SetUniformBuffer)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                U32 size;
                cmdList->ReadBuffer(size);
                U8* data = JOJO_NEW_ARRAY(U8, size);
                cmdList->ReadBufferPtr((U8*)data, size);
                mProtocol->SetUniformBuffer(handle, data, size);
                break;
            }
            CASECMD(GFXCommand::CreateDescriptorSetLayout)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);

                U32 size;
                cmdList->ReadBuffer(size);
                PODVector<DescriptorSetLayoutBinding> bindings;
                bindings.Resize(size);
                for (U32 i = 0; i < size; ++i)
                {
                    cmdList->ReadBuffer(bindings[i]);
                }
                mProtocol->CreateDescriptorSetLayout(handle, bindings);
                break;
            }
            CASECMD(GFXCommand::CreatePipelineLayout)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);
                GPUPipelineLayoutCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                createInfo.setLayoutHandles.Resize(createInfo.numLayouts);
                for (U32 i = 0; i < createInfo.numLayouts; ++i)
                {
                    cmdList->ReadBuffer(createInfo.setLayoutHandles[i]);
                }
                mProtocol->CreatePipelineLayout(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::CreateDescriptorSets)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);
                GPUDescriptorSetsCreateInfo createInfo;
                cmdList->ReadBuffer(createInfo);
                mProtocol->CreateDescriptorSets(handle, createInfo);
                break;
            }
            CASECMD(GFXCommand::UpdateDescriptorSets)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);
                GPUDescriptorSetsUpdateInfo updateInfo;
                cmdList->ReadBuffer(updateInfo);
                updateInfo.updateSets.Resize(updateInfo.num);
                for (U32 i = 0; i < updateInfo.num; ++i)
                {
                    cmdList->ReadBuffer(updateInfo.updateSets[i]);
                }
                mProtocol->UpdateDescriptorSets(handle, updateInfo);
                break;
            }
            CASECMD(GFXCommand::QueueSubmit)
            {
                GPUResourceHandle handle;
                cmdList->ReadBuffer<GPUResourceHandle>(handle);
                U32 size;
                cmdList->ReadBuffer(size);
                U8* data = JOJO_NEW_ARRAY(U8, size);
                cmdList->ReadBufferPtr(data, size);
                mProtocol->QueueSubmit(handle, size, data);
                break;
            }
            default:
            {
                LOGERROR("CMD NOT FOUND: %s\n", GFXCommand(command));
                break;
            }
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