#pragma once
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "Camera.h"
#include "../Base/ObjectDefines.h"
#include "../Math/Matrix4x4.h"
namespace Joestar {
	class Renderer : public Object {
		REGISTER_OBJECT(Renderer, Object);
	public:
		explicit Renderer(EngineContext* ctx);
		Mesh* mesh;
		Material* mat;
		Matrix4x4f model;
		void Render(Camera cam);
	};
}