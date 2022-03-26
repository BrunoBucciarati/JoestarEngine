#pragma once
#include "../GraphicDefines.h"
#include "../GPUResource.h"
#include "../../Resource/Resource.h"
#include "../../Container/Str.h"
namespace Joestar {
	class ShaderReflection;

	class Shader : public Resource {
		REGISTER_OBJECT(Shader, Resource)
	public:
		explicit Shader(EngineContext* ctx);
		void SetShader(String n, ShaderStage stage = ShaderStage::VS_PS);
		void SetLanguage(ShaderLanguage lang);
		bool LoadFile(const String& path);
	private:
		String GetDirectoryByLang(ShaderLanguage lang);
		ShaderStage mStage;
		ShaderLanguage mLang{ShaderLanguage::GLSL};
		ShaderLanguage mTargetLang;
		UniquePtr<ShaderReflection> mReflection;
	};
}