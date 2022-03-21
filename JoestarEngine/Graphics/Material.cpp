#include "Material.h"

namespace Joestar {
	Material::~Material() {
		DELETE_OBJECT(program);
	}
	void Material::SetTexture(Texture* tex, U8 slot) {
		if (slot >= textures.Size())
			textures.Resize(slot + 1);
		textures[slot] = tex;
	}

	void Material::SetPBR() {
		program->shader->SetShader("pbrsh");
		textures.Reserve(4);
		const char* texNames[] = {
			"basecolor.png",
			"normal.png",
			"metallic.png",
			"roughness.png"
		};
		String path = "Textures/pbr/rustediron/";
		for (int i = 0; i < 4; ++i) {
			textures.Push(NEW_OBJECT(Texture2D));
			String pngFile = path + texNames[i];
			textures[i]->TextureFromImage(pngFile);
		}
	}

	void Material::SetDefault() {
		program->shader->SetShader("test");
		textures.Push(NEW_OBJECT(Texture2D));
		String path = "Models/viking_room/viking_room.png";
		textures[0]->TextureFromImage(path);
		//textures[0]->hasMipMap = false;
	}

	void Material::SetPlaneMat() {
		program->shader->SetShader("test");
		textures.Push(NEW_OBJECT(Texture2D));
		String path = "Textures/marble.jpg";
		textures[0]->TextureFromImage(path);
	}

}