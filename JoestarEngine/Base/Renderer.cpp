#include "Renderer.h"
#include "../Misc/Application.h"
#include "../Graphics/Graphics.h"
namespace Joestar {
	Renderer::Renderer(EngineContext* ctx) : Super(ctx) {
		model = Matrix4x4f::identity;
		//for test now
		shaderName = "test";
	}
	void Renderer::Render(Camera cam) {
		if (mesh) {
			Graphics* graphics = GetSubsystem<Graphics>();
			graphics->UseShader(shaderName);
			graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, model);
			graphics->UpdateVertexBuffer(mesh->GetVB());
			graphics->UpdateIndexBuffer(mesh->GetIB());
			graphics->DrawIndexed();
		}
	}
}