#include "Renderer.h"
#include "../Misc/Application.h"
#include "../Graphics/Graphics.h"
#include "../Base/GameObject.h"
#include "../Graphics/PipelineState.h"
namespace Joestar {
	void Renderer::Init()
	{
		mGraphics = GetSubsystem<Graphics>();
	}
	Renderer::~Renderer(){}

	void Renderer::Render(Camera* cam)
	{
		//if (mesh && mat) {
		//	graphics->UpdateMaterial(mat);
		//	graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, gameObject->GetAfflineTransform());
		//	graphics->DrawMesh(mesh, mat);
		//}
	}

	void Renderer::RenderToShadowMap()
	{
		//if (mesh) {
		//	graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, gameObject->GetAfflineTransform());
		//	graphics->DrawMesh(mesh, mat);
		//}
	}
}