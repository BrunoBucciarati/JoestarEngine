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

		//û�ҵ���Ӧ�ģ�����һ���µ�PSO
		SharedPtr<GraphicsPipelineState>& pso = mPSOs.EmplaceBack();
		pso = JOJO_NEW(GraphicsPipelineState, MEMORY_GFX_STRUCT);
		pso->SetRenderPass(pass);
		pso->SetViewport(cb->GetViewport());
		//��Щֵ����default�ģ�������˲���������Ҫ������Щ�µ��߼� --todo
		pso->SetDepthStencilState(mGraphics->GetDefaultDepthStencilState());
		pso->SetColorBlendState(mGraphics->GetDefaultColorBlendState());
		pso->SetMultiSampleState(mGraphics->GetDefaultMultiSampleState());
		pso->SetRasterizationState(mGraphics->GetDefaultRasterizationState());
		mGraphics->CreateGraphicsPipelineState(pso);
		return pso;
	}
}