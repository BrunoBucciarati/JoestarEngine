#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include "Shader/Shader.h"
#include "../Container/Str.h"
#include "../Container/Vector.h"
#include "../Core/Object.h"
#include "Texture2D.h"

namespace Joestar {
	class Material : public Object {
		REGISTER_OBJECT(Material, Object);
	public:
		explicit Material(EngineContext* ctx) : Super(ctx)
		{}
		void SetDefault();
		void SetPBR();
		void SetPlaneMat();

		void SetShader(const String& name, ShaderStage stage = ShaderStage::VS_PS);

		void SetShader(Shader* shader)
		{
			mShader = shader;
		}

		void SetTexture(Texture* tex, U8 slot = 0);
		Shader* GetShader()
		{ 
			return mShader;
		}
		String GetShaderName()
		{
			return mShader->GetName();
		}
		Vector<Texture*>& GetTextures()
		{
			return mTextures;
		}
		Texture* GetTexture(U32 slot)
		{
			return mTextures[slot];
		}
	private:
		Vector<Texture*> mTextures;
		SharedPtr<Shader> mShader;
	};
}
#endif