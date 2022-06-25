#pragma once
#include "../IO/MemoryWriter.h"
#include <atomic>
#include "GraphicsDebugs.h"
#if DEBUG_CMD
#include "../IO/Log.h"
#endif
namespace Joestar
{
	enum class GFXCommand
	{
		CreateSwapChain = 0,
		CreateCommandPool,
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
		CreateSampler,
		CreateTexture,
		SubmitCommandBuffer,
		QueueSubmitCommandBuffer,
		Present,
		CommandCount
	};

#if DEBUG_CMD
	static const char* DebugCommands[]
	{
		"CreateSwapChain",
		"CreateCommandPool",
		"CreateCommandBuffer",
		"CreateFrameBuffer",
		"CreateImage",
		"CreateImageView",
		"CreateIndexBuffer",
		"CreateVertexBuffer",
		"CreateUniformBuffer",
		"CreateRenderPass",
		"CreateColorBlendState",
		"CreateDepthStencilState",
		"CreateRasterizationState",
		"CreateMultiSampleState",
		"CreateGraphicsPipelineState",
		"CreateComputePipelineState",
		"CreateMemory",
		"CreateSyncObjects",
		"CreateDescriptorPool",
		"CreateShader",
		"CreateShaderProgram",
		"SetUniformBuffer",
		"CreatePipelineLayout",
		"CreateDescriptorSetLayout",
		"CreateDescriptorSets",
		"UpdateDescriptorSets",
		"CreateSampler",
		"CreateTexture",
		"SubmitCommandBuffer",
		"QueueSubmitCommandBuffer",
		"Present"
	};
	static_assert(sizeof(DebugCommands) / sizeof(const char*) == (U32)GFXCommand::CommandCount, "Command Not Match");
#endif
	class GFXCommandList : public MemoryReadWriter
	{
	public:
		void WriteCommand(GFXCommand t)
		{
#if DEBUG_CMD
			//LOGWARN("Write Command: %s\n", DebugCommands[U32(t)]);
#endif
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