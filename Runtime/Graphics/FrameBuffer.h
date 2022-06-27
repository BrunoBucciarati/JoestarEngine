#pragma once
#include "Texture2D.h"
#include "PipelineState.h"
namespace Joestar
{
	class FrameBuffer : public GPUResource
	{
		GET_SET_STATEMENT_INITVALUE(U32, Width, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Height, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Layers, 1);
		GET_SET_STATEMENT_INITVALUE(U32, MultiSample, 1);
		GET_SET_STATEMENT(SharedPtr<GPUImageView>, DepthStencil);
		GET_SET_STATEMENT(SharedPtr<RenderPass>, RenderPass);
	public:
		U32 GetNumColorAttachments();
		GPUImageView* GetColorAttachment(U32 idx = 0);
	private:
		SharedPtr<GPUImageView> mColorAttachments[4]{};
	};
}