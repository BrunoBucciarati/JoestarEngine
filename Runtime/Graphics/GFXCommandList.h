#pragma once
#include "../IO/MemoryWriter.h"
#include "../IO/Log.h"
#include <atomic>
namespace Joestar
{
#define DEBUG_CMD(CMD_TYPE) \
    LOGWARN("CPU CMD:%s\n", #CMD_TYPE);

	enum class GFXCommand
	{
		CreateSwapChain = 0,
		CreateCommandBuffer,
		CreateFrameBuffer,
		CreateImage,
		CreateImageView,
		CreateIndexBuffer,
		CreateVertexBuffer,
		CreateUniformBuffer,
		CreateRenderPass,
		CreateColorBlendState,
		CreateDepthStencilState,
		CreateRasterizationState,
		CreateMultiSampleState,
		CreateGraphicsPipelineState,
		CreateComputePipelineState,
		CreateMemory,
		CreateSyncObjects,
		CreateDescriptorPool,
		CreateShader,
		CreateShaderProgram,
		SetUniformBuffer,
		CreatePipelineLayout,
		CreateDescriptorSetLayout,
		CreateDescriptorSets,
		UpdateDescriptorSets,
		CommandCount
	};
	class GFXCommandList : public MemoryReadWriter
	{
	public:
		void WriteCommand(GFXCommand t)
		{
			if (t == GFXCommand::CreateShaderProgram)
			{
				int err = 1;
			}
			WriteBuffer<GFXCommand>(t);
		}

		bool ReadCommand(GFXCommand& t)
		{
			return ReadBuffer<GFXCommand>(t);
		}

		void Flush()
		{
			MemoryReadWriter::Flush();
			readFlag = true;
		}
		//当这个标志为true时意味着可读
		std::atomic_bool readFlag{ false };
	};
}