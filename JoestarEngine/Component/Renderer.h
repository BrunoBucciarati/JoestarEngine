#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Base/Camera.h"
#include "../Base/ObjectDefines.h"
#include "../Math/Matrix4x4.h"
#include "../Component/Component.h"
namespace Joestar {
	class Renderer : public Component {
		REGISTER_COMPONENT(Renderer);
	public:
		void Init();
		Mesh* mesh;
		Material* mat;
		void Render(Camera cam);
	};
}