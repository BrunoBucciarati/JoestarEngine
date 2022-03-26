#include "Material.h"

namespace Joestar {
	Material::~Material()
	{
	}

	void Material::SetShader(const String& name, ShaderStage stage)
	{
		mShader = NEW_OBJECT(Shader);
		mShader->SetShader(name, stage);
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

}