#include "Renderer.h"
#include "../Misc/Application.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/MaterialInstance.h"
#include "../Base/GameObject.h"
namespace Joestar {
	void Renderer::Init()
	{
		mGraphics = GetSubsystem<Graphics>();
		//mShaderProgram = NEW_OBJECT(ShaderProgram);
		//mMaterial = NEW_OBJECT(Material);
		//mDescriptorSets = JOJO_NEW(DescriptorSets);
	}
	Renderer::~Renderer(){}

	void Renderer::SetMaterial(const String& vs, const String& ps)
	{
		Material* mat = NEW_OBJECT(Material);
		mat->SetShader(vs, ShaderStage::VS);
		mat->SetShader(ps, ShaderStage::PS);
		if (mat->IsValid())
			mMaterial = NEW_OBJECT(MaterialInstance, mat);
		else
			LOGERROR("Material Is Not Valid!");
	}

	void Renderer::SetMaterial(const String& vsps)
	{
		SetMaterial(vsps, vsps);
	}
	//void Renderer::SetShader(const String& name, ShaderStage stage)
	//{
	//	mShaderProgram->SetShader(stage, name);
	//	if (mShaderProgram->IsValid())
	//	{
	//		//设置逐材质的参数描述到材质中
	//		auto layout = mShaderProgram->GetDescriptorSetLayout(UniformFrequency::OBJECT);
	//		//如果存在逐Object的uniform，那么设置到renderer中
	//		if (layout)
	//		{
	//			mDescriptorSets->AllocFromLayout(layout);
	//			mGraphics->CreateDescriptorSets(mDescriptorSets);

	//			//对描述符分配对应的UB
	//			mUniformBuffers.Clear();
	//			mUniformBuffers.Resize(mDescriptorSets->Size());
	//			for (U32 i = 0; i < layout->GetNumBindings(); ++i)
	//			{
	//				DescriptorSetLayoutBinding* binding = layout->GetLayoutBinding(i);
	//				mUniformBuffers[i] = JOJO_NEW(UniformBuffer, MEMORY_GFX_STRUCT);
	//				mUniformBuffers[i]->AllocFromBinding(binding);
	//				mUniformBuffers[i]->SetFrequency(UniformFrequency::OBJECT);
	//				mGraphics->CreateUniformBuffer(mUniformBuffers[i]);
	//				mDescriptorSets->SetBindingUniformBuffer(binding->binding, mUniformBuffers[i]);
	//			}
	//		}

	//		//设置逐材质的uniform
	//		mMaterial->SetDescriptorSetLayout(mShaderProgram->GetDescriptorSetLayout(UniformFrequency::BATCH));
	//		mMaterial->AllocDescriptorSets();
	//	}
	//}

	//void Renderer::SetUniformBuffer(PerObjectUniforms uniform, U8* data)
	//{
	//	auto layout = mShaderProgram->GetDescriptorSetLayout(UniformFrequency::OBJECT);
	//	if (!layout)
	//		return;
	//	DescriptorSetLayoutBinding::Member member;
	//	U32 binding = layout->GetUniformMemberAndBinding((U32)uniform, member);
	//	mUniformBuffers[binding]->SetData(member.offset, member.size, data);
	//}

	SharedPtr<GraphicsPipelineState> Renderer::GetPipelineState(CommandBuffer* cb)
	{
		return nullptr;
	}
}