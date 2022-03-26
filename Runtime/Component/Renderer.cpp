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

	void Renderer::Render(CommandBuffer* cb)
	{
		auto& pso = GetPipelineState(cb);
	}

	void Renderer::RenderToShadowMap()
	{
	}


	SharedPtr<GraphicsPipelineState>& Renderer::GetPipelineState(CommandBuffer* cb)
	{
		RenderPass* pass = cb->GetRenderPass();
		for (auto& pso : mPSOs)
		{
			if (pso->GetRenderPass() == pass && pso->GetViewport() == cb->GetViewport())
			{
				return pso;
			}
		}

		//没找到对应的，创建一个新的PSO
		SharedPtr<GraphicsPipelineState>& pso = mPSOs.EmplaceBack();
		pso = JOJO_NEW(GraphicsPipelineState, MEMORY_GFX_STRUCT);
		pso->SetRenderPass(pass);
		pso->SetViewport(cb->GetViewport());
		//这些值先用default的，后面加了材质设置需要这里做些新的逻辑 --todo
		pso->SetDepthStencilState(mGraphics->GetDefaultDepthStencilState());
		pso->SetColorBlendState(mGraphics->GetDefaultColorBlendState());
		pso->SetMultiSampleState(mGraphics->GetDefaultMultiSampleState());
		pso->SetRasterizationState(mGraphics->GetDefaultRasterizationState());
		mGraphics->CreateGraphicsPipelineState(pso);
		return pso;
	}
}