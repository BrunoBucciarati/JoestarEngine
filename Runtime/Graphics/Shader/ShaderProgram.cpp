#include "ShaderProgram.h"
#include "../Graphics.h"

namespace Joestar
{
	U32 GetStageIndex(ShaderStage s)
	{
		U32 stage = (U32)s;
		U32 index = 0;
		for (; index < stage; ++index)
		{
			if (1 << index & stage)
				break;
		}
		return index;
	}
	bool IsValidProgram(U32 mask)
	{
		if (mask == (U32)ShaderStage::VS_PS || mask == (U32)ShaderStage::VS_GS_PS || mask == (U32)ShaderStage::CS)
		{
			return true;
		}
		return false;
	}

	void ShaderProgram::SetShader(ShaderStage s, Shader* vs, Shader* ps)
	{
		if (s != ShaderStage::VS_PS)
		{
			return;
		}
		SetShader(ShaderStage::VS, vs);
		SetShader(ShaderStage::PS, ps);
	}
	//最终的接口
	void ShaderProgram::SetShader(ShaderStage s, Shader* shader)
	{
		U32 stage = (U32)s;
		U32 index = GetStageIndex(s);
		if (mShaders.Size() < index + 1)
		{
			mShaders.Resize(index + 1);
		}
		mShaders[index] = shader;
		mStageMask |= stage;
		bValid = IsValidProgram(mStageMask);
		if (bValid)
		{
			GetSubsystem<Graphics>()->CreateShaderProgram(this);
		}
	}
	void ShaderProgram::SetShader(ShaderStage s, const String& name)
	{
		if (s == ShaderStage::VS_PS)
		{
			SetShader(s, name, name);
			return;
		}
		Shader* shader = NEW_OBJECT(Shader);
		shader->SetShader(name, s);
		SetShader(s, shader);
	}
	void ShaderProgram::SetShader(ShaderStage s, const String& vs, const String& ps)
	{
		if (s != ShaderStage::VS_PS)
		{
			return;
		}
		SetShader(ShaderStage::VS, vs);
		SetShader(ShaderStage::PS, ps);
	}
	Shader* ShaderProgram::GetShader(ShaderStage s)
	{
		U32 index = GetStageIndex(s);
		return GetShader(index);
	}
	Shader* ShaderProgram::GetShader(U32 index)
	{
		if (mShaders.Size() < index + 1)
		{
			return nullptr;
		}
		return mShaders[index];
	}
	U32 ShaderProgram::GetNumStages()
	{
		if (!bValid)
			return 0;
		if (mStageMask == (U32)ShaderStage::VS_PS)
			return 2;

		if (mStageMask == (U32)ShaderStage::VS_GS_PS)
			return 3;

		if (mStageMask == (U32)ShaderStage::CS)
			return 1;
		return 0;
	}
}