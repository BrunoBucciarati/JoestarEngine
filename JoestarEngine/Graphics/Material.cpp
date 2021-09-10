#include "Material.h"

namespace Joestar {
	Material::~Material() {
		DELETE_OBJECT(program);
	}
	void Material::SetTexture(Texture* tex, U8 slot) {
		if (slot >= textures.size())
			textures.resize(slot + 1);
		textures[slot] = tex;
	}

	void Material::SetPBR() {
		program->shader->SetShader("pbr");
		textures.reserve(4);
		const char* texNames[] = {
			"basecolor.png",
			"metallic.png",
			"normal.png",
			"roughness.png"
		};
		std::string path = "Textures/pbr/rustediron/";
		for (int i = 0; i < 4; ++i) {
			textures.push_back(NEW_OBJECT(Texture2D));
			std::string pngFile = path + texNames[i];
			textures[i]->TextureFromImage(pngFile);
		}
	}

	void Material::SetDefault() {
		program->shader->SetShader("test");
		textures.push_back(NEW_OBJECT(Texture2D));
		std::string path = "Models/viking_room/viking_room.png";
		textures[0]->TextureFromImage(path);
		//textures[0]->hasMipMap = false;
	}

}