#include "CommandBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GPUCreateInfos.h"
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
	void CommandBuffer::BeginRenderPass(RenderPass* pass, FrameBuffer* fb)
	{
		mPass = pass;
		mEncoder.WriteCommand(CommandBufferCMD::BeginRenderPass);
		
		RenderPassBeginInfo beginInfo{
			pass->GetHandle(),
			fb->GetHandle(),
			mViewport->rect,
			2
		};
		mEncoder.WriteBuffer(beginInfo);
		ClearValue clearColorValue;
		clearColorValue.color = { 0, 0, 0, 0 };
		mEncoder.WriteBuffer(clearColorValue);
		ClearValue clearDSValue;
		clearDSValue.depthStencil = { 1.0F, 0 };
		mEncoder.WriteBuffer(clearDSValue);
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
	void CommandBuffer::BindDescriptorSets(U32 set, PipelineLayout* pipelineLayout, DescriptorSets* sets)
	{
		mEncoder.WriteCommand(CommandBufferCMD::BindDescriptorSets);
		mEncoder.WriteBuffer(pipelineLayout->GetHandle());
		mEncoder.WriteBuffer(sets->GetHandle());
	}
	void CommandBuffer::DrawIndexed(U32 count, U32 indexStart, U32 vertStart)
	{
		mEncoder.WriteCommand(CommandBufferCMD::DrawIndexed);
		mEncoder.WriteBuffer(count);
		mEncoder.WriteBuffer(indexStart);
		mEncoder.WriteBuffer(vertStart);
	}
}