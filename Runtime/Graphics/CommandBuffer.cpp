#include "CommandBuffer.h"

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
}