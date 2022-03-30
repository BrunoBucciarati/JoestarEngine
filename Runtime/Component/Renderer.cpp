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
			//设置逐材质的参数描述到材质中
			mMaterial->SetDescriptorSetLayout(mShaderProgram->GetDescriptorSetLayout(UniformFrequency::BATCH));
			auto layout = mShaderProgram->GetDescriptorSetLayout(UniformFrequency::OBJECT);
			mDescriptorSets->AllocFromLayout(layout);

			//对描述符分配对应的UB
			mUniformBuffers.Clear();
			mUniformBuffers.Resize(mDescriptorSets->Size());
			for (U32 i = 0; i < layout->GetNumBindings(); ++i)
			{
				DescriptorSetLayoutBinding* binding = layout->GetLayoutBinding(i);
				U32 hash = binding->Hash();
				UniformType type = { UniformFrequency::OBJECT };
				mUniformBuffers[i] = JOJO_NEW(UniformBuffer, MEMORY_GFX_STRUCT);
				mUniformBuffers[i]->SetSet((U32)UniformFrequency::OBJECT);
				mUniformBuffers[i]->SetBinding(binding->binding);
				mUniformBuffers[i]->SetSize(binding->size);
				mGraphics->CreateUniformBuffer(mUniformBuffers[i]);
				mDescriptorSets->GetDescriptorSetByBinding(binding->binding).ub = mUniformBuffers[i];
			}
		}
	}

	void Renderer::SetUniformBuffer(PerObjectUniforms uniform, U8* data)
	{
		//检查Layout中是否有这个描述符
		//DescriptorSetLayoutBinding::Member member;
		//U32 binding = mShaderProgram->GetUniformMemberAndBinding((U32)UniformFrequency::OBJECT, (U32)uniform, member);

		auto layout = mShaderProgram->GetDescriptorSetLayout(UniformFrequency::OBJECT);
		DescriptorSet& set = mDescriptorSets->GetDescriptorSetByID((U32)uniform);

		mUniformBuffers[set.binding]->SetData(set.offset, set.size, data);
		//if (!set.ub)
		//	set.ub = mGraphics->CreateUniformBuffer((U32)uniform, { GetPerObjectUniformDataType(uniform), UniformFrequency::OBJECT });
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

		////没找到对应的，创建一个新的PSO
		//SharedPtr<GraphicsPipelineState>& pso = mPSOs.EmplaceBack();
		//pso = JOJO_NEW(GraphicsPipelineState, MEMORY_GFX_STRUCT);
		//pso->SetRenderPass(pass);
		//pso->SetShaderProgram(mMaterial->GetShaderProgram());
		//pso->SetViewport(cb->GetViewport());
		////这些值先用default的，后面加了材质设置需要这里做些新的逻辑 --todo
		//pso->SetDepthStencilState(mDepthStencilState);
		//pso->SetColorBlendState(mColorBlendState);
		//pso->SetMultiSampleState(mMultiSampleState);
		//pso->SetRasterizationState(mRasterizationState);
		//mGraphics->CreateGraphicsPipelineState(pso);
		//return pso;
	}
}