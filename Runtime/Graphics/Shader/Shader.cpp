#include "Shader.h"
#include "../../IO/FileSystem.h"
#include "../../Misc/GlobalConfig.h"
#include "../Graphics.h"
#include "ShaderReflection.h"
#include <d3dcompiler.h>

namespace Joestar {
	Shader::Shader(EngineContext* ctx): Super(ctx)
	{

		auto* cfg = GetSubsystem<GlobalConfig>();
		GFX_API api = (GFX_API)cfg->GetConfig<U32>(CONFIG_GFX_API);
		if (api == GFX_API_OPENGL)
		{
			mTargetLang = ShaderLanguage::GLSL;
			mLang = ShaderLanguage::GLSL;
		}
		else if (api == GFX_API_VULKAN)
		{
			mTargetLang = ShaderLanguage::SPIRV;
			mLang = ShaderLanguage::GLSL;
		}
		else if (api == GFX_API_D3D11 || api == GFX_API_D3D12)
		{
			mTargetLang = ShaderLanguage::HLSL;
			mLang = ShaderLanguage::HLSL;
		}
		SetLanguage(mLang);
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
			//HLSL先编程Blob
			if (mLang == ShaderLanguage::HLSL)
			{

				UINT flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
				flags |= D3DCOMPILE_DEBUG;
#endif
				bool ret = Super::LoadFile(path + GetShaderSuffix(mLang, mStage));
				if (ret)
				{
					ID3D10Blob* compiledShader{ nullptr };
					ID3D10Blob* compilationMsgs{ nullptr };
					if (mStage == ShaderStage::VS)
					{
						HRESULT hr = D3DCompile(mFile->GetBuffer(), mFile->Size(), "TEST", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS",
							"vs_5_0", flags, 0, &compiledShader, &compilationMsgs);
					}
					else if (mStage == ShaderStage::PS)
					{
						HRESULT hr = D3DCompile(mFile->GetBuffer(), mFile->Size(), "TEST", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS",
							"ps_5_0", flags, 0, &compiledShader, &compilationMsgs);
					}
					else if (mStage == ShaderStage::CS)
					{
						HRESULT hr = D3DCompile(mFile->GetBuffer(), mFile->Size(), "TEST", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS",
							"cs_5_0", flags, 0, &compiledShader, &compilationMsgs);
					}
					// compilationMsgs中包含错误或警告信息
					if (compilationMsgs != 0)
					{
						const char* errorMsg = (const char*)compilationMsgs->GetBufferPointer();
						LOGERROR("%s\n", errorMsg);
					}
					mBlob = compiledShader;
					mReflection = JOJO_NEW(ShaderReflection, MEMORY_GFX_STRUCT);
					mReflection->ReflectHLSL(mBlob, mStage);

					GetSubsystem<Graphics>()->CreateShader(this);
				}
				else
				{
					return ret;
				}
			}
			else
			{
				return Super::LoadFile(path + GetShaderSuffix(mLang, mStage));
			}
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