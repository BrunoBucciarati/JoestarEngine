#include "RenderAPIProtocol.h"
#include "Window.h"

#define GET_STRUCT_BY_HANDLE_FROM_VECTOR(_VAR, _TYP, _HANDLE, _VEC) \
    if (_HANDLE + 1 > _VEC.Size()) _VEC.Resize(_HANDLE + 1); \
    _TYP##& _VAR =  _VEC[_HANDLE];

#define GET_STRUCT_BY_HANDLE(_VAR, _TYP, _HANDLE) \
    GET_STRUCT_BY_HANDLE_FROM_VECTOR(_VAR, _TYP, _HANDLE, m##_TYP##s);

#define DEBUG_CMD 1
#if DEBUG_CMD
#define CASECMD(CMD_TYPE) \
    case CMD_TYPE: \
    LOGWARN("Frame:%d CMD:%s\n",mFrameIndex, #CMD_TYPE);
#else
#define CASECMD(CMD_TYPE) \
    case CMD_TYPE:
#endif

namespace Joestar {
	void RenderAPIProtocol::SetWindow(Window* wd)
	{
		window = wd;
	}

	void RenderAPIProtocol::CreateMemory(GPUResourceHandle handle, U32 size, U8* data)
	{
		if (handle + 1 > mMemories.Size())
			mMemories.Resize(handle + 1);
		mMemories[handle].size = size;
		mMemories[handle].data = data;
	}

	void RenderAPIProtocol::CreateColorBlendState(GPUResourceHandle handle, GPUColorBlendStateCreateInfo& createInfo)
	{
		GET_STRUCT_BY_HANDLE_FROM_VECTOR(state, GPUColorBlendStateCreateInfo, handle, mColorBlendStates);
		state = createInfo;
	}
	void RenderAPIProtocol::CreateDepthStencilState(GPUResourceHandle handle, GPUDepthStencilStateCreateInfo& createInfo)
	{
		GET_STRUCT_BY_HANDLE_FROM_VECTOR(state, GPUDepthStencilStateCreateInfo, handle, mDepthStencilStates);
		state = createInfo;
	}
	void RenderAPIProtocol::CreateRasterizationState(GPUResourceHandle handle, GPURasterizationStateCreateInfo& createInfo)
	{
		GET_STRUCT_BY_HANDLE_FROM_VECTOR(state, GPURasterizationStateCreateInfo, handle, mRasterizationStates);
		state = createInfo;
	}
	void RenderAPIProtocol::CreateMultiSampleState(GPUResourceHandle handle, GPUMultiSampleStateCreateInfo& createInfo)
	{
		GET_STRUCT_BY_HANDLE_FROM_VECTOR(state, GPUMultiSampleStateCreateInfo, handle, mMultiSampleStates);
		state = createInfo;
	}
	void RenderAPIProtocol::CreateShaderProgram(GPUResourceHandle handle, GPUShaderProgramCreateInfo& createInfo)
	{
		GET_STRUCT_BY_HANDLE_FROM_VECTOR(state, GPUShaderProgramCreateInfo, handle, mShaderPrograms);
		state = createInfo;
	}
	void RenderAPIProtocol::QueueSubmitCommandBuffer(GPUResourceHandle handle, U32 size, U8* data)
	{
		//GET_STRUCT_BY_HANDLE_FROM_VECTOR(encoder, CommandEncoder, handle, mCommandEncoders);
		CommandEncoder encoder;
		encoder.SetData(data, size);
		CommandBufferCMD cmd;
		//在这里循环，然后调抽象的接口，免得每个API中要写一遍循环
		while (encoder.ReadBuffer(cmd))
		{
			RecordCommand(cmd, encoder, handle);
		}
		QueueSubmit(handle);
	}

	void RenderAPIProtocol::RecordCommand(CommandBufferCMD& cmd, CommandEncoder& encoder, GPUResourceHandle cbHandle)
	{
#define HANDLE_COMMAND(_TYP) \
	CASECMD(CommandBufferCMD::##_TYP) \
	{\
		GPUResourceHandle handle; \
		encoder.ReadBuffer(handle); \
		CB##_TYP(cbHandle, handle); \
		break; \
	}
		switch (cmd)
		{
			CASECMD(CommandBufferCMD::Begin)
			{
				CBBegin(cbHandle);
				break;
			}
			CASECMD(CommandBufferCMD::End)
			{
				CBEnd(cbHandle);
				break;
			}
			CASECMD(CommandBufferCMD::BeginRenderPass)
			{
				RenderPassBeginInfo beginInfo;
				encoder.ReadBuffer(beginInfo);
				beginInfo.clearValues.Resize(beginInfo.numClearValues);
				for (U32 i = 0; i < beginInfo.numClearValues; ++i)
				{
					encoder.ReadBuffer(beginInfo.clearValues[i]);
				}
				CBBeginRenderPass(cbHandle, beginInfo);
				break;
			}
			HANDLE_COMMAND(EndRenderPass)
			HANDLE_COMMAND(BindGraphicsPipeline)
			HANDLE_COMMAND(BindComputePipeline)
			HANDLE_COMMAND(BindIndexBuffer)
			HANDLE_COMMAND(Draw)
			CASECMD(CommandBufferCMD::BindDescriptorSets)
			{
				GPUResourceHandle handle;
				encoder.ReadBuffer(handle);
				GPUResourceHandle handle2;
				encoder.ReadBuffer(handle2);
				CBBindDescriptorSets(cbHandle, handle, handle2);
				break;
			}
			CASECMD(CommandBufferCMD::BindVertexBuffer)
			{
				GPUResourceHandle handle;
				encoder.ReadBuffer(handle);
				U32 binding = 0;
				encoder.ReadBuffer(binding);
				CBBindVertexBuffer(cbHandle, handle, binding);
				break;
			}
			CASECMD(CommandBufferCMD::DrawIndexed)
			{
				U32 count;
				U32 indexStart;
				U32 vertStart;
				encoder.ReadBuffer(count);
				encoder.ReadBuffer(indexStart);
				encoder.ReadBuffer(vertStart);
				CBDrawIndexed(cbHandle, count, indexStart, vertStart);
				break;
			}
			default:
			{
				LOGERROR("Unknown CommandBuffer CMD: %d", cmd);
				break;
			}
		}
	}
}