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
		CopyBuffer,
		Submit,
		CommandCount
	};

	enum class CopyBufferType
	{
		VB = 0,
		IB,
		UB
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
	
	class CommandPool : public GPUResource
	{
		GET_SET_STATEMENT(GPUQueue, Queue);
	};

	class CommandBuffer : public GPUResource
	{
		GET_SET_STATEMENT(GPUQueue, Queue);
		GET_SET_STATEMENT(Viewport*, Viewport);
		GET_SET_STATEMENT(DescriptorSets*, PassDescriptorSets);
		GET_SET_STATEMENT(CommandPool*, Pool);
	public:
		void Begin();
		void End();
		void BeginRenderPass(RenderPass* pass, FrameBuffer* fb);
		void EndRenderPass(RenderPass* pass);
		void BindPipelineState(GraphicsPipelineState* pass);
		void BindVertexBuffer(VertexBuffer* vb, U32 binding = 0);
		void BindIndexBuffer(IndexBuffer* ib);
		void BindDescriptorSets(U32 set, PipelineLayout* pipelineLayout, DescriptorSets* sets);
		void BindDescriptorSets(UniformFrequency freq, PipelineLayout* pipelineLayout, DescriptorSets* sets)
		{
			return BindDescriptorSets(U32(freq), pipelineLayout, sets);
		}
		void DrawIndexed(U32 num, U32 indexStart = 0, U32 vertStart = 0);
		void Draw(U32 num, U32 vertStart = 0);
		void CopyBuffer(CopyBufferType, GPUResourceHandle);
		void Submit();
		RenderPass* GetRenderPass() const
		{
			return mPass;
		}
		CommandEncoder& GetEncoder()
		{
			return mEncoder;
		}
		bool IsRecording() const
		{
			return bRecording;
		}

	private:
		RenderPass* mPass;
		CommandEncoder mEncoder;
		GraphicsPipelineState* mGraphicsPSO;
		bool bRecording{ false };
	};
}