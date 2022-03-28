#include "Material.h"
#include "Graphics.h"

namespace Joestar {
	Material::Material(EngineContext* ctx) : Super(ctx),
		mShaderProgram(NEW_OBJECT(ShaderProgram)),
		mGraphics(GetSubsystem<Graphics>())
	{}
	Material::~Material()
	{
	}

	void Material::SetShader(const String& name, ShaderStage stage)
	{
		mShaderProgram->SetShader(stage, name);
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

	void Material::SetUniformBuffer(PerObjectUniforms uniform, float* data)
	{
		bool bFound = false;
		for (U32 i = 0; i < mDescriptorSets.Size(); ++i)
		{
			if (mDescriptorSets[i].ub->GetID() == (U32)uniform)
			{
				mGraphics->SetUniformBuffer(mDescriptorSets[i].ub, data);
				bFound = true;
				break;
			}
		}

		if (bFound)
			return;
		//todo 这里要反射拿到binding
		DescriptorSet set{};
		set.binding = 0;
		set.ub = mGraphics->CreateUniformBuffer((U32)uniform, { GetPerObjectUniformDataType(uniform), UniformFrequency::OBJECT });
		mDescriptorSets.Push(set);
	}
}