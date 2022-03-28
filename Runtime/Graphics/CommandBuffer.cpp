#include "CommandBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
namespace Joestar
{
	void CommandBuffer::Begin()
	{
		mEncoder.WriteCommand(CommandBufferCMD::Begin);
	}
	void CommandBuffer::End()
	{
		mGraphicsPSO = nullptr;
		mPass = nullptr;
		mEncoder.WriteCommand(CommandBufferCMD::End);
	}
	void CommandBuffer::BeginRenderPass(RenderPass* pass)
	{
		mPass = pass;
		mEncoder.WriteCommand(CommandBufferCMD::BeginRenderPass);
		mEncoder.WriteBuffer(pass->handle);
	}
	void CommandBuffer::EndRenderPass(RenderPass* pass)
	{
		mPass = pass;
		mEncoder.WriteCommand(CommandBufferCMD::EndRenderPass);
		mEncoder.WriteBuffer(pass->handle);
	}
	void CommandBuffer::BindPipelineState(GraphicsPipelineState* pso)
	{
		mGraphicsPSO = pso;
		mEncoder.WriteCommand(CommandBufferCMD::BindGraphicsPipeline);
		mEncoder.WriteBuffer(mGraphicsPSO->handle);
	}
	void CommandBuffer::BindVertexBuffer(VertexBuffer* vb, U32 binding)
	{
		mEncoder.WriteCommand(CommandBufferCMD::BindVertexBuffer);
		mEncoder.WriteBuffer(vb->GetGPUHandle());
		mEncoder.WriteBuffer(binding);
	}
	void CommandBuffer::BindIndexBuffer(IndexBuffer* ib)
	{
		mEncoder.WriteCommand(CommandBufferCMD::BindIndexBuffer);
		mEncoder.WriteBuffer(ib->GetGPUHandle());
	}
	void CommandBuffer::BindDescriptorSets(Vector<DescriptorSet>& sets)
	{
		mEncoder.WriteCommand(CommandBufferCMD::BindDescriptorSets);
		mEncoder.WriteBuffer(sets.Size());
		for (U32 i = 0; i < sets.Size(); ++i)
		{
			mEncoder.WriteBuffer(sets[i]);
		}
	}
	void CommandBuffer::DrawIndexed(U32 count, U32 indexStart, U32 vertStart)
	{
		mEncoder.WriteCommand(CommandBufferCMD::DrawIndexed);
		mEncoder.WriteBuffer(count);
		mEncoder.WriteBuffer(indexStart);
		mEncoder.WriteBuffer(vertStart);
	}
}