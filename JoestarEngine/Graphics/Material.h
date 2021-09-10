#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include "Shader/Shader.h"
#include <vector>
#include "../Base/Object.h"
#include "ProgramCPU.h"
#include "Texture2D.h"

namespace Joestar {
	class Material : public Object {
		REGISTER_OBJECT(Material, Object)
	public:
		explicit Material(EngineContext* ctx):Super(ctx) {
			program = NEW_OBJECT(ProgramCPU);
		}
	//GPUProgram* program;
	//Material() :program(GPUProgram::CreateProgram()) {}
	void SetDefault();
	void SetPBR();

	void SetShader(Shader* shader) {
		program->shader = shader;
	}

	void SetTexture(Texture* tex, U8 slot = 0);
	Shader* GetShader() { return program->shader; }
	std::string& GetShaderName() { return program->shader->GetName(); }
	ProgramCPU* GetProgram() { return program; }
	std::vector<Texture*>& GetTextures() {
		return textures;
	}
	Texture* GetTexture(U32 slot) {
		return textures[slot];
	}
	private:
		ProgramCPU* program;
		std::vector<Texture*> textures;
	};
}
#endif