#include "Renderer.h"
#include "../Misc/Application.h"
#include "../Graphics/Graphics.h"
#include "../Base/GameObject.h"
namespace Joestar {
	void Renderer::Init()
	{
		mGraphics = GetSubsystem<Graphics>();
		//mDepthStencilState = mGraphics->GetDefaultDepthStencilState();
		//mColorBlendState = mGraphics->GetDefaultColorBlendState();
		//mMultiSampleState = mGraphics->GetDefaultMultiSampleState();
		//mRasterizationState = mGraphics->GetDefaultRasterizationState();
	}
	Renderer::~Renderer(){}

	void Renderer::RenderToShadowMap()
	{
	}


	SharedPtr<GraphicsPipelineState> Renderer::GetPipelineState(CommandBuffer* cb)
	{
		return nullptr;
		//RenderPass* pass = cb->GetRenderPass();
		//for (auto& pso : mPSOs)
		//{
		//	if (pso->GetRenderPass() == pass && pso->GetViewport() == cb->GetViewport())
		//	{
		//		return pso;
		//	}
		//}

		////û�ҵ���Ӧ�ģ�����һ���µ�PSO
		//SharedPtr<GraphicsPipelineState>& pso = mPSOs.EmplaceBack();
		//pso = JOJO_NEW(GraphicsPipelineState, MEMORY_GFX_STRUCT);
		//pso->SetRenderPass(pass);
		//pso->SetShaderProgram(mMaterial->GetShaderProgram());
		//pso->SetViewport(cb->GetViewport());
		////��Щֵ����default�ģ�������˲���������Ҫ������Щ�µ��߼� --todo
		//pso->SetDepthStencilState(mDepthStencilState);
		//pso->SetColorBlendState(mColorBlendState);
		//pso->SetMultiSampleState(mMultiSampleState);
		//pso->SetRasterizationState(mRasterizationState);
		//mGraphics->CreateGraphicsPipelineState(pso);
		//return pso;
	}
}