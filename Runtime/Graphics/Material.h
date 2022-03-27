#pragma once
#include "Shader/ShaderProgram.h"
#include "../Container/Str.h"
#include "../Container/Vector.h"
#include "../Core/Object.h"
#include "Texture2D.h"

namespace Joestar {
	class Material : public Object {
		REGISTER_OBJECT(Material, Object);
	public:
		explicit Material(EngineContext* ctx);
		void SetDefault();
		void SetPBR();
		void SetPlaneMat();

		void SetShader(const String& name, ShaderStage stage = ShaderStage::VS_PS);

		void SetShader(Shader* shader, ShaderStage stage = ShaderStage::VS_PS)
		{
			mShaderProgram->SetShader(stage, shader);
		}

		void SetTexture(Texture* tex, U8 slot = 0);
		Shader* GetShader(ShaderStage stage)
		{ 
			return mShaderProgram->GetShader(stage);
		}
		String GetShaderName(ShaderStage stage)
		{
			return GetShader(stage)->GetName();
		}
		Vector<Texture*>& GetTextures()
		{
			return mTextures;
		}
		Texture* GetTexture(U32 slot)
		{
			return mTextures[slot];
		}
		ShaderProgram* GetShaderProgram()
		{
			return mShaderProgram;
		}
	private:
		Vector<Texture*> mTextures;
		SharedPtr<ShaderProgram> mShaderProgram;
	};
}
