#include "Renderer.h"
#include "../Misc/Application.h"
#include "../Graphics/Graphics.h"
namespace Joestar {
	Renderer::Renderer(EngineContext* ctx) : Super(ctx) {
		model = Matrix4x4f::identity;
		//for test now
	}
	void Renderer::Render(Camera cam) {
		if (mesh && mat) {
			Graphics* graphics = GetSubsystem<Graphics>();
			graphics->UpdateMaterial(mat);
			//graphics->UseShader(mat->GetShaderName());
			//graphics->UpdateTexture(mat->GetShaderName());
			graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, model);
			graphics->UpdateVertexBuffer(mesh->GetVB(mat->GetShader()->GetVertexAttributeFlag()));
			graphics->UpdateIndexBuffer(mesh->GetIB());
			graphics->DrawIndexed();
		}
	}
}