#pragma once
#include "PipelineState.h"
#include "../IO/MemoryWriter.h"
namespace Joestar {
	enum class CommandBufferCMD
	{
		Begin = 0,
		End,
		BeginRenderPass,
		EndRenderPass,
		BindGraphicsPipeline,
		BindComputePipeline,
		SetIndexBuffer,
		SetVertexBuffer,
		Draw,
		DrawIndexed,
		CommandCount
	};

	class CommandEncoder : public MemoryReadWriter
	{
	public:
		void WriteCommand(CommandBufferCMD t)
		{
			WriteBuffer<CommandBufferCMD>(t);
		}

		bool ReadCommand(CommandBufferCMD& t)
		{
			return ReadBuffer<CommandBufferCMD>(t);
		}
	};


	class CommandBuffer : public GPUResource
	{
		GET_SET_STATEMENT(GPUQueue, Queue);
		GET_SET_STATEMENT(Viewport*, Viewport);
	public:
		void Begin();
		void End();
		void BeginRenderPass(RenderPass* pass);
		void EndRenderPass(RenderPass* pass);
		void BindPipelineState(GraphicsPipelineState* pass);
		RenderPass* GetRenderPass() const
		{
			return mPass;
		}

	private:
		RenderPass* mPass;
		CommandEncoder mEncoder;
		GraphicsPipelineState* mGraphicsPSO;
	};
}