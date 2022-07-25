#include "MaterialInstance.h"
#include "Material.h"
#include "Descriptor.h"
#include "UniformBuffer.h"
#include "Graphics.h"
#include "GraphicDefines.h"
#include "Texture.h"

namespace Joestar {
	MaterialInstance::MaterialInstance(EngineContext* ctx, Material* mat) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>()),
		mMaterial(mat)
	{
		if (mat->IsValid())
		{
			Init();
		}
	}
	MaterialInstance::~MaterialInstance()
	{}

	void MaterialInstance::Init()
	{
		ShaderProgram* program = mMaterial->GetShaderProgram();

		if (program->IsValid())
		{
			//创建逐Object的描述符
			auto layout = program->GetDescriptorSetLayout(UniformFrequency::OBJECT);
			if (layout)
			{
				if (!mObjectDescriptorSets)
				{
					mObjectDescriptorSets = JOJO_NEW(DescriptorSets);
				}
				mObjectDescriptorSets->AllocFromLayout(layout);
				mGraphics->CreateDescriptorSets(mObjectDescriptorSets);

				//对描述符分配对应的UB
				mObjectUniformBuffers.Clear();
				mObjectUniformBuffers.Resize(mObjectDescriptorSets->Size());
				for (U32 i = 0; i < layout->GetNumBindings(); ++i)
				{
					DescriptorSetLayoutBinding* binding = layout->GetLayoutBinding(i);
					mObjectUniformBuffers[i] = JOJO_NEW(UniformBuffer, MEMORY_GFX_STRUCT);
					mObjectUniformBuffers[i]->AllocFromBinding(binding);
					mObjectUniformBuffers[i]->SetFrequency(UniformFrequency::OBJECT);
					mGraphics->CreateUniformBuffer(mObjectUniformBuffers[i]);
					mObjectDescriptorSets->SetBindingUniformBuffer(binding->binding, mObjectUniformBuffers[i]);
				}
			}

			//逐材质的描述符
			layout = program->GetDescriptorSetLayout(UniformFrequency::BATCH);
			if (layout)
			{
				if (!mBatchDescriptorSets)
				{
					mBatchDescriptorSets = JOJO_NEW(DescriptorSets);
				}
				mBatchDescriptorSets->AllocFromLayout(layout);
				mGraphics->CreateDescriptorSets(mBatchDescriptorSets);

				//对描述符分配对应的UB
				mBatchUniformBuffers.Clear();
				mBatchUniformBuffers.Resize(mBatchDescriptorSets->Size());
				for (U32 i = 0; i < layout->GetNumBindings(); ++i)
				{
					DescriptorSetLayoutBinding* binding = layout->GetLayoutBinding(i);
					if (binding->type == DescriptorType::UNIFORM_BUFFER)
					{
						mBatchUniformBuffers[i] = JOJO_NEW(UniformBuffer, MEMORY_GFX_STRUCT);
						mBatchUniformBuffers[i]->AllocFromBinding(binding);
						mBatchUniformBuffers[i]->SetFrequency(UniformFrequency::BATCH);
						mGraphics->CreateUniformBuffer(mBatchUniformBuffers[i]);
						mBatchDescriptorSets->SetBindingUniformBuffer(binding->binding, mBatchUniformBuffers[i]);
					}
					else if (binding->type == DescriptorType::COMBINED_IMAGE_SAMPLER)
					{
						if (mTextures.Size() < i + 1)
						{
							mTextures.Resize(i + 1);
						}
						if (!mTextures[i])
						{
							mTextures[i] = JOJO_NEW(Texture(mContext), MEMORY_TEXTURE);
						}
						mTextures[i]->SetFrequency(UniformFrequency::BATCH);
						mBatchDescriptorSets->SetBindingTexture(binding->binding, mTextures[i]);
					}
				}
			}
		}

		//创建一个默认的PSO，渲染时候还要重新设置状态使用
		mPiplelineState = JOJO_NEW(GraphicsPipelineState, MEMORY_GFX_STRUCT);
		//mPiplelineState->SetRenderPass(pass);
		//mPiplelineState->SetViewport(cb->GetViewport());
		mPiplelineState->SetShaderProgram(mMaterial->GetShaderProgram());
		mPiplelineState->SetPipelineLayout(mMaterial->GetShaderProgram()->GetPipelineLayout());
		//这些值先用default的，后面加了材质设置需要这里做些新的逻辑 --todo
		mPiplelineState->SetDepthStencilState(mGraphics->GetDefaultDepthStencilState());
		mPiplelineState->SetColorBlendState(mGraphics->GetDefaultColorBlendState());
		mPiplelineState->SetMultiSampleState(mGraphics->GetDefaultMultiSampleState());
		mPiplelineState->SetRasterizationState(mGraphics->GetDefaultRasterizationState());
	}

	bool MaterialInstance::Update()
	{
		bool bUpdate = false;
		if (bDepthStencilStateDirty)
		{
			mGraphics->CreateDepthStencilState(mDepthStencilState);
			mPiplelineState->SetDepthStencilState(mDepthStencilState);
			bUpdate = true;
			bDepthStencilStateDirty = false;
		}
		if (bRasterStateDirty)
		{
			mGraphics->CreateRasterizationState(mRasterizationState);
			mPiplelineState->SetRasterizationState(mRasterizationState);
			bUpdate = true;
			bRasterStateDirty = false;
		}
		//mGraphics->UpdateDescriptorSets(mDescriptorSets);
		//if (bUpdate)
			//ReHash();
		return bUpdate;
	}

	void MaterialInstance::SetTexture(Texture* tex, U32 binding)
	{
		if (binding >= mTextures.Size())
			mTextures.Resize(binding + 1);
		mTextures[binding] = tex;

		mBatchDescriptorSets->SetBindingTexture(binding, tex);
	}

	void MaterialInstance::SetDepthCompareOp(CompareOp op)
	{
		if (!mDepthStencilState)
		{
			mDepthStencilState = JOJO_NEW(DepthStencilState, MEMORY_GFX_STRUCT);
		}
		mDepthStencilState->SetDepthCompareOp(op);
		bDepthStencilStateDirty = true;
	}

	void MaterialInstance::SetTessellationControlPoints(U32 num)
	{
		mTessellationControlPoints = num;
	}

	SharedPtr<DepthStencilState> MaterialInstance::GetDepthStencilState() const
	{
		return mDepthStencilState;
	}

	void MaterialInstance::SetUniformBuffer(PerObjectUniforms uniform, U8* data)
	{
		auto layout = mMaterial->GetShaderProgram()->GetDescriptorSetLayout(UniformFrequency::OBJECT);
		if (!layout)
			return;
		DescriptorSetLayoutBinding::Member member;
		U32 binding = layout->GetUniformMemberAndBinding((U32)uniform, member);
		if (binding != U32_MAX)
			mObjectUniformBuffers[binding]->SetData(member.offset, member.size, data);
	}

	void MaterialInstance::SetUniformBuffer(PerBatchUniforms uniform, U8* data)
	{
		auto layout = mMaterial->GetShaderProgram()->GetDescriptorSetLayout(UniformFrequency::BATCH);
		if (!layout)
			return;
		DescriptorSetLayoutBinding::Member member;
		U32 binding = layout->GetUniformMemberAndBinding((U32)uniform, member);
		mBatchUniformBuffers[binding]->SetData(member.offset, member.size, data);
	}

	void MaterialInstance::UpdateDescriptorSets()
	{
		for (auto& ub : mBatchUniformBuffers)
		{
			if (ub)
				mGraphics->SetUniformBuffer(ub);
		}
		if (mBatchDescriptorSets)
			mGraphics->UpdateDescriptorSets(mBatchDescriptorSets);

		for (auto& ub : mObjectUniformBuffers)
		{
			if (ub)
				mGraphics->SetUniformBuffer(ub);
		}
		if (mObjectDescriptorSets)
			mGraphics->UpdateDescriptorSets(mObjectDescriptorSets);
	}

	ShaderProgram* MaterialInstance::GetShaderProgram()
	{
		if (mMaterial)
		{
			return mMaterial->GetShaderProgram();
		}
		return nullptr;
	}


	void MaterialInstance::SetPolygonMode(PolygonMode mode)
	{
		if (!mRasterizationState)
		{
			mRasterizationState = JOJO_NEW(RasterizationState, MEMORY_GFX_STRUCT);
		}
		mRasterizationState->SetPolygonMode(mode);
		bRasterStateDirty = true;
	}

}