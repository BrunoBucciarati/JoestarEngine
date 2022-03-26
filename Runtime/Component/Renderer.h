#pragma once
#include "../Core/Minimal.h"
#include "../Component/Component.h"
#include "../Graphics/PipelineState.h"
#include "../Graphics/CommandBuffer.h"
namespace Joestar {
	class Graphics;
	class GraphicsPipelineState;
	class Renderer : public Component {
		REGISTER_COMPONENT_ROOT(Renderer);
	public:
		void Init();
		virtual void Render(CommandBuffer*) = 0;
		void RenderToShadowMap();
		SharedPtr<GraphicsPipelineState>& GetPipelineState(CommandBuffer* pass);
	private:
		WeakPtr<Graphics> mGraphics;
		Vector<SharedPtr<GraphicsPipelineState>> mPSOs;
		SharedPtr<ColorBlendState> mColorBlendState;
		SharedPtr<DepthStencilState> mDepthStencilState;
		SharedPtr<MultiSampleState> mMultiSampleState;
		SharedPtr<RasterizationState> mRasterizationState;
	};
}