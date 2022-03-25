#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include "Shader/Shader.h"
#include "../Container/Str.h"
#include "../Container/Vector.h"
#include "../Core/Object.h"
#include "ProgramCPU.h"
#include "Texture2D.h"

namespace Joestar {
	class Material : public Object {
		REGISTER_OBJECT(Material, Object)
	public:
		explicit Material(EngineContext* ctx):Super(ctx) {
			//program = NEW_OBJECT(ProgramCPU);
		}
	//GPUProgram* program;
	//Material() :program(GPUProgram::CreateProgram()) {}
	void SetDefault();
	void SetPBR();
	void SetPlaneMat();

	void SetShader(Shader* shader) {
		//program->shader = shader;
	}

	void SetTexture(Texture* tex, U8 slot = 0);
	Shader* GetShader() { return nullptr; }// program->shader; }
	String GetShaderName()
	{
		String a = "";
		return a;
	}
	ProgramCPU* GetProgram() { return program; }
	Vector<Texture*>& GetTextures() {
		return textures;
	}
	Texture* GetTexture(U32 slot) {
		return textures[slot];
	}
	private:
		ProgramCPU* program;
		Vector<Texture*> textures;
	};
}
#endif