#include "Shader.h"
#include "../../IO/FileSystem.h"
#include "../../Misc/GlobalConfig.h"
#include "../Graphics.h"
#include "ShaderReflection.h"

namespace Joestar {
	Shader::Shader(EngineContext* ctx): Super(ctx)
	{
		SetLanguage(mLang);

		auto* cfg = GetSubsystem<GlobalConfig>();
		GFX_API api = (GFX_API)cfg->GetConfig<U32>(CONFIG_GFX_API);
		if (api == GFX_API_OPENGL)
		{
			mTargetLang = ShaderLanguage::GLSL;
		}
		else if (api == GFX_API_VULKAN)
		{
			mTargetLang = ShaderLanguage::SPIRV;
		}
		else if (api == GFX_API_D3D11 || api == GFX_API_D3D12)
		{
			mTargetLang = ShaderLanguage::HLSL;
		}
	}

	Shader::~Shader()
	{}

	void Shader::SetShader(String name, ShaderStage stage)
	{
		SetName(name);
		mStage = stage;
		LoadFile(mDirectory + name);
	}

	bool Shader::LoadFile(const String& path)
	{
		if (mLang == mTargetLang)
		{
			return Super::LoadFile(path + GetShaderSuffix(mLang, mStage));
		}
		//Compile To Spirv
		else if (mTargetLang == ShaderLanguage::SPIRV)
		{
			String targetDirectory = GetDirectoryByLang(mTargetLang);
			String spvFile = targetDirectory + GetName() + GetShaderSuffix(mTargetLang, mStage);
			String compileSpvCmd = mDirectory + "glslc.exe " + (path + GetShaderSuffix(mLang, mStage)) + " -o " + spvFile;
			system(compileSpvCmd.CString());

			if (!Super::LoadFile(spvFile))
			{
				LOGERROR("Compile Spirv Failed! : CMD = %s", compileSpvCmd.CString());
			}
			mReflection = JOJO_NEW(ShaderReflection, MEMORY_GFX_STRUCT);
			mReflection->ReflectSpirv(mFile, mStage);

			GetSubsystem<Graphics>()->CreateShader(this);
		}
		return false;
	}

	String Shader::GetDirectoryByLang(ShaderLanguage lang)
	{
		String ret;
		auto* fs = GetSubsystem<FileSystem>();
		if (ShaderLanguage::GLSL == lang)
		{
			ret = fs->GetShaderDirAbsolute() + "GLSL/";
		}
		else if (ShaderLanguage::HLSL == lang)
		{
			ret = fs->GetShaderDirAbsolute() + "HLSL/";
		}
		else if (ShaderLanguage::SPIRV == lang)
		{
			ret = fs->GetShaderDirAbsolute() + "SPIRV/";
		}
		return ret;
	}

	void Shader::SetLanguage(ShaderLanguage lang)
	{
		mLang = lang;
		mDirectory = GetDirectoryByLang(lang);
	}
}