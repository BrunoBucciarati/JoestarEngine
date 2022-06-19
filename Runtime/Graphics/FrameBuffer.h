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
		GET_SET_STATEMENT(SharedPtr<Texture2D>, DepthStencil);
		GET_SET_STATEMENT(SharedPtr<RenderPass>, RenderPass);
	public:
		U32 GetNumColorAttachments();
		Texture2D* GetColorAttachment(U32 idx = 0);
	private:
		SharedPtr<Texture2D> mColorAttachments[4]{};
	};
}