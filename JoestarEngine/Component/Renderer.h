#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Base/Camera.h"
#include "../Core/ObjectDefines.h"
#include "../Math/Matrix4x4.h"
#include "../Component/Component.h"
namespace Joestar {
	class Graphics;
	class Renderer : public Component {
		REGISTER_COMPONENT(Renderer);
	public:
		void Init();
		void Render(Camera* cam);
		void RenderToShadowMap();
	private:
		Graphics* graphics;
	};
}