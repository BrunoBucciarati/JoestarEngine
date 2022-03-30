#pragma once
#include "PipelineState.h"
#include "FrameBuffer.h"
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
		BindIndexBuffer,
		BindVertexBuffer,
		BindDescriptorSets,
		PushConstants,
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

	class IndexBuffer;
	class VertexBuffer;

	class CommandBuffer : public GPUResource
	{
		GET_SET_STATEMENT(GPUQueue, Queue);
		GET_SET_STATEMENT(Viewport*, Viewport);
	public:
		void Begin();
		void End();
		void BeginRenderPass(RenderPass* pass, FrameBuffer* fb);
		void EndRenderPass(RenderPass* pass);
		void BindPipelineState(GraphicsPipelineState* pass);
		void BindVertexBuffer(VertexBuffer* vb, U32 binding = 0);
		void BindIndexBuffer(IndexBuffer* ib);
		void BindDescriptorSets(U32 set, PipelineLayout* pipelineLayout, DescriptorSets* sets);
		void DrawIndexed(U32 num, U32 indexStart = 0, U32 vertStart = 0);
		void Draw(U32 num, U32 vertStart = 0);
		RenderPass* GetRenderPass() const
		{
			return mPass;
		}
		CommandEncoder& GetEncoder()
		{
			return mEncoder;
		}

	private:
		RenderPass* mPass;
		CommandEncoder mEncoder;
		GraphicsPipelineState* mGraphicsPSO;
	};
}