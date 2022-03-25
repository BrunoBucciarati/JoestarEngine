#include "Renderer.h"
#include "../Misc/Application.h"
#include "../Graphics/Graphics.h"
#include "../Base/GameObject.h"
namespace Joestar {
	void Renderer::Init()
	{
		mGraphics = GetSubsystem<Graphics>();
	}
	Renderer::~Renderer(){}

	void Renderer::Render(RenderPass* pass)
	{
		//if (mesh && mat) {
		//	graphics->UpdateMaterial(mat);
		//	graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, gameObject->GetAfflineTransform());
		//	graphics->DrawMesh(mesh, mat);
		//}
		auto& pso = GetPipelineState(pass);
	}

	void Renderer::RenderToShadowMap()
	{
		//if (mesh) {
		//	graphics->UpdateBuiltinMatrix(BUILTIN_MATRIX_MODEL, gameObject->GetAfflineTransform());
		//	graphics->DrawMesh(mesh, mat);
		//}
	}


	SharedPtr<GraphicsPipelineState>& Renderer::GetPipelineState(RenderPass* pass)
	{
		if (pass->handle + 1 > mPSOs.Size())
		{
			mPSOs.Resize(pass->handle + 1);
		}
		SharedPtr<GraphicsPipelineState>& pso = mPSOs[pass->handle];
		if (!pso)
		{
			pso = JOJO_NEW(GraphicsPipelineState, MEMORY_GFX_STRUCT);
			pso->SetRenderPass(pass);
			mGraphics->CreateGraphicsPipelineState(pso);
		}
		return pso;
	}
}