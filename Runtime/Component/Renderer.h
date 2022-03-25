#pragma once
#include "../Core/Minimal.h"
#include "../Component/Component.h"
#include "../Graphics/PipelineState.h"
namespace Joestar {
	class Graphics;
	class RenderPass;
	class GraphicsPipelineState;
	class Renderer : public Component {
		REGISTER_COMPONENT_ROOT(Renderer);
	public:
		void Init();
		void Render(RenderPass*);
		void RenderToShadowMap();
		SharedPtr<GraphicsPipelineState>& GetPipelineState(RenderPass* pass);
	private:
		WeakPtr<Graphics> mGraphics;
		Vector<SharedPtr<GraphicsPipelineState>> mPSOs;
	};
}