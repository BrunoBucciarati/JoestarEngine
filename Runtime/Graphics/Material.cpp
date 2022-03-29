#include "Material.h"
#include "Graphics.h"

namespace Joestar {
	Material::Material(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>())
	{}
	Material::~Material()
	{
	}

	void Material::SetTexture(Texture* tex, U8 slot)
	{
		if (slot >= mTextures.Size())
			mTextures.Resize(slot + 1);
		mTextures[slot] = tex;
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
			mTextures[i]->TextureFromImage(pngFile);
		}
	}

	void Material::SetDefault()
	{
		mTextures.Push(NEW_OBJECT(Texture2D));
		String path = "Models/viking_room/viking_room.png";
		mTextures[0]->TextureFromImage(path);
	}

	void Material::SetPlaneMat()
	{
		mTextures.Push(NEW_OBJECT(Texture2D));
		String path = "Textures/marble.jpg";
		mTextures[0]->TextureFromImage(path);
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
		////todo ����Ҫ�����õ�binding
		//if (mDescriptorSets.Empty())
		//{
		//	mDescriptorSets.Resize(mDescriptorSetLayout.GetNumBindings());
		//}
		////���Layout���Ƿ������������
		//DescriptorSetLayoutBinding::Member member;
		//U32 binding = mDescriptorSetLayout.GetUniformMemberAndBinding((U32)uniform, member);

		//DescriptorSet& set = mDescriptorSets[binding];
		//set.binding = binding;
		//set.set = (U32)UniformFrequency::BATCH;
		//set.ub = mGraphics->CreateUniformBuffer((U32)uniform, { GetPerBatchUniformDataType(uniform), UniformFrequency::BATCH });
		//mDescriptorSets.Push(set);
	}
}