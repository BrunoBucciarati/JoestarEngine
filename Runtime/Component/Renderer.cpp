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
		mShaderProgram = NEW_OBJECT(ShaderProgram);
		mMaterial = NEW_OBJECT(Material);
		mDescriptorSets = JOJO_NEW(DescriptorSets);
	}
	Renderer::~Renderer(){}

	void Renderer::RenderToShadowMap()
	{
	}

	void Renderer::SetShader(const String& name, ShaderStage stage)
	{
		mShaderProgram->SetShader(stage, name);
		if (mShaderProgram->IsValid())
		{
			//��������ʵĲ���������������
			mMaterial->SetDescriptorSetLayout(mShaderProgram->GetDescriptorSetLayout(UniformFrequency::BATCH));
			mDescriptorSets->AllocFromLayout(mShaderProgram->GetDescriptorSetLayout(UniformFrequency::OBJECT));
		}
	}

	void Renderer::SetUniformBuffer(PerObjectUniforms uniform, float* data)
	{
		//���Layout���Ƿ������������
		DescriptorSetLayoutBinding::Member member;
		U32 binding = mShaderProgram->GetUniformMemberAndBinding((U32)UniformFrequency::OBJECT, (U32)uniform, member);

		U32 idx = mDescriptorSets->GetDescriptorSetBinding((U32)uniform);
		DescriptorSet& set = mDescriptorSets->GetDescriptorSet(idx);
		set.binding = binding;
		set.set = (U32)UniformFrequency::OBJECT;
		set.ub = mGraphics->CreateUniformBuffer((U32)uniform, { GetPerObjectUniformDataType(uniform), UniformFrequency::OBJECT });
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