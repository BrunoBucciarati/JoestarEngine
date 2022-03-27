#include "ShaderProgram.h"
#include "../Graphics.h"
#include "../GraphicDefines.h"
#include "ShaderReflection.h"

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

	ShaderProgram::ShaderProgram(EngineContext* ctx) : Super(ctx)
	{
		mDescriptorLayouts.Reserve(MAX_DESCRIPTOR_SETS);
	}
	ShaderProgram::~ShaderProgram()
	{}

	void ShaderProgram::CheckValid()
	{
		bValid = IsValidProgram(mStageMask);
	}

	void ShaderProgram::CollectInputAndDescriptors()
	{
		//从VS/CS收集输入
		mInputAttributes.Clear();
		if (mStageMask & (U32)ShaderStage::VS)
		{
			Shader* shader = GetShader(ShaderStage::VS);
			auto* reflection = shader->GetReflection();
			auto& refInputAttrs = reflection->GetInputAttributes();
			mInputAttributes.Reserve(refInputAttrs.Size());
			for (auto& ip : refInputAttrs)
			{
				mInputAttributes.Push(ip);
			}
		}
		else if (mStageMask & (U32)ShaderStage::CS)
		{
			//todo
		}
		//从不同的stage收集合并描述符
		mDescriptorLayouts.Clear();
		if (mStageMask == (U32)ShaderStage::VS_PS)
		{
			U32 maxSets = 0;
			for (auto& shader : mShaders)
			{
				auto* reflection = shader->GetReflection();
				maxSets = Max(maxSets, reflection->GetNumDescriptorSetLayouts());
			}
			mDescriptorLayouts.Resize(maxSets);

			for (auto& shader : mShaders)
			{
				auto* reflection = shader->GetReflection();
				Vector<DescriptorSetLayout>& setLayouts = reflection->GetDescriptorSetLayouts();
				for (U32 setIdx = 0; setIdx < setLayouts.Size(); ++setIdx)
				{
					DescriptorSetLayout& curSetLayout = mDescriptorLayouts[setIdx];
					for (U32 bindingIdx = 0; bindingIdx < setLayouts[setIdx].GetNumBindings(); ++bindingIdx)
					{
						DescriptorSetLayoutBinding& binding = setLayouts[setIdx].GetLayoutBindings(bindingIdx);
						if (!curSetLayout.AddBinding(binding))
						{
							bValid = false;
							LOGERROR("PROGRAM DESCRIPTOR ERROR");
							return;
						}
					}
				}
			}
		}
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
		CheckValid();
		if (bValid)
		{
			CollectInputAndDescriptors();
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