#include "Material.h"
#include "Graphics.h"

namespace Joestar {
	Material::Material(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>()),
		mDescriptorSets(JOJO_NEW(DescriptorSets, MEMORY_GFX_STRUCT))
	{}
	Material::~Material()
	{}

	void Material::SetTexture(Texture* tex, U32 binding)
	{
		if (binding >= mTextures.Size())
			mTextures.Resize(binding + 1);
		mTextures[binding] = tex;
	}

	void Material::SetPBR()
	{
		//program->shader->SetShader("pbrsh");
		mTextures.Reserve(4);
		const char* texNames[] = {
			"basecolor.png",
			"normal.png",
			"metallic.png",
			"roughness.png"
		};
		String path = "Textures/pbr/rustediron/";
		for (int i = 0; i < 4; ++i) {
			mTextures.Push(NEW_OBJECT(Texture2D));
			String pngFile = path + texNames[i];
			//mTextures[i]->TextureFromImage(pngFile);
		}
	}

	void Material::SetDefault()
	{
		mTextures.Push(NEW_OBJECT(Texture2D));
		String path = "Models/viking_room/viking_room.png";
		//mTextures[0]->TextureFromImage(path);
	}

	void Material::SetPlaneMat()
	{
		Texture2D* texture = NEW_OBJECT(Texture2D);
		mTextures.Push(texture);
		String path = "Textures/marble.jpg";
		Image image(mContext);
		image.Load(path);
		texture->SetImage(&image);
	}

	void Material::AllocDescriptorSets()
	{
		mDescriptorSets->AllocFromLayout(mDescriptorSetLayout);
		mGraphics->CreateDescriptorSets(mDescriptorSets);

		//对描述符分配对应的UB
		mUniformBuffers.Clear();
		mUniformBuffers.Resize(mDescriptorSets->Size());
		mTextures.Resize(mDescriptorSets->Size());
		for (U32 i = 0; i < mDescriptorSetLayout->GetNumBindings(); ++i)
		{
			DescriptorSetLayoutBinding* binding = mDescriptorSetLayout->GetLayoutBinding(i);
			if (binding->type == DescriptorType::UNIFORM_BUFFER)
			{
				mUniformBuffers[i] = JOJO_NEW(UniformBuffer, MEMORY_GFX_STRUCT);
				mUniformBuffers[i]->AllocFromBinding(binding);
				mUniformBuffers[i]->SetFrequency(UniformFrequency::BATCH);
				mGraphics->CreateUniformBuffer(mUniformBuffers[i]);
				mDescriptorSets->SetBindingUniformBuffer(binding->binding, mUniformBuffers[i]);
			}
			else if (binding->type == DescriptorType::COMBINED_IMAGE_SAMPLER)
			{
				if (!mTextures[i])
				{
					mTextures[i] = JOJO_NEW(Texture(mContext), MEMORY_TEXTURE);
				}
				mTextures[i]->SetFrequency(UniformFrequency::BATCH);
				mDescriptorSets->SetBindingTexture(binding->binding, mTextures[i]);
			}
		}
	}

	void Material::UpdateDescriptorSets()
	{
		mGraphics->UpdateDescriptorSets(mDescriptorSets);
	}

	void Material::SetUniformBuffer(PerBatchUniforms uniform, float* data)
	{
		bool bFound = false;
		//for (U32 i = 0; i < mDescriptorSets->Size(); ++i)
		//{
		//	if (mDescriptorSets[i].ub->GetID() == (U32)uniform)
		//	{
		//		mGraphics->SetUniformBuffer(mDescriptorSets[i].ub, data);
		//		bFound = true;
		//		break;
		//	}
		//}

		//if (bFound)
		//	return;
		////todo 这里要反射拿到binding
		//if (mDescriptorSets.Empty())
		//{
		//	mDescriptorSets.Resize(mDescriptorSetLayout.GetNumBindings());
		//}
		////检查Layout中是否有这个描述符
		//DescriptorSetLayoutBinding::Member member;
		//U32 binding = mDescriptorSetLayout.GetUniformMemberAndBinding((U32)uniform, member);

		//DescriptorSet& set = mDescriptorSets[binding];
		//set.binding = binding;
		//set.set = (U32)UniformFrequency::BATCH;
		//set.ub = mGraphics->CreateUniformBuffer((U32)uniform, { GetPerBatchUniformDataType(uniform), UniformFrequency::BATCH });
		//mDescriptorSets.Push(set);
	}
}