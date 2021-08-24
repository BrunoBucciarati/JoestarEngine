#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include "Shader/Shader.h"
#include <vector>
#include "../Base/Object.h"
#include "Texture.h"

namespace Joestar {
	class Material : public Object {
		REGISTER_OBJECT(Material, Object)
	public:
		explicit Material(EngineContext* ctx):Super(ctx) {}
	//GPUProgram* program;
	//Material() :program(GPUProgram::CreateProgram()) {}
	void SetDefault() {
		if (!shader) {
			shader = NEW_OBJECT(Shader);
			shader->SetName("test");
		}
		textures.push_back(NEW_OBJECT(Texture));
		std::string path = "Models/viking_room/viking_room.png";
		textures[0]->TextureFromImage(path);
	}
	std::string& GetShaderName() { return shader->GetName(); }
	Shader* GetShader() { return shader; }
	std::vector<Texture*>& GetTextures() { return textures;  }
	private:
		Shader* shader;
		std::vector<Texture*> textures;
	};
}
#endif