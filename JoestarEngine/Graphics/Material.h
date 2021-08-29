#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include "Shader/Shader.h"
#include <vector>
#include "../Base/Object.h"
#include "Texture.h"
#include "ProgramCPU.h"

namespace Joestar {
	class Material : public Object {
		REGISTER_OBJECT(Material, Object)
	public:
		explicit Material(EngineContext* ctx):Super(ctx) {
			program = NEW_OBJECT(ProgramCPU);
		}
	//GPUProgram* program;
	//Material() :program(GPUProgram::CreateProgram()) {}
	void SetDefault() {
		if (!program->shader) {
			program->shader = NEW_OBJECT(Shader);
			program->shader->SetName("test");
		}
		textures.push_back(NEW_OBJECT(Texture));
		std::string path = "Models/viking_room/viking_room.png";
		textures[0]->TextureFromImage(path);
	}

	void SetShader(Shader* shader) {
		program->shader = shader;
	}
	std::string& GetShaderName() { return program->shader->GetName(); }
	Shader* GetShader() { return program->shader; }
	ProgramCPU* GetProgram() { return program; }
	std::vector<Texture*>& GetTextures() {
		return textures;
	}
	private:
		ProgramCPU* program;
		std::vector<Texture*> textures;
	};
}
#endif