#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Base/Camera.h"
#include "../Core/ObjectDefines.h"
#include "../Component/Component.h"
#include "../Component/Component.h"
namespace Joestar {
	class Graphics;
	class GraphicsPipelineState;
	class Renderer : public Component {
		REGISTER_COMPONENT_ROOT(Renderer);
	public:
		void Init();
		void Render(Camera* cam);
		void RenderToShadowMap();
	private:
		Graphics* mGraphics;
	};
}