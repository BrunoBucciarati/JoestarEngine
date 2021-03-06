#include "ShaderProgram.h"
#include "../Graphics.h"
#include "../GraphicDefines.h"
#include "ShaderReflection.h"
#include "../PipelineState.h"

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
		if (mask == (U32)ShaderStage::VS_PS || mask == (U32)ShaderStage::VS_HS_DS_PS || mask == (U32)ShaderStage::VS_GS_PS || mask == (U32)ShaderStage::CS)
		{
			return true;
		}
		return false;
	}

	ShaderProgram::ShaderProgram(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>())
	{
		//Reserve(MAX_DESCRIPTOR_SETS);
		mPipelineLayout = JOJO_NEW(PipelineLayout);
	}
	ShaderProgram::~ShaderProgram()
	{}

	void ShaderProgram::CheckValid()
	{
		bValid = IsValidProgram(mStageMask);
	}

	U32 ShaderProgram::GetNumDescriptorBindings(U32 set) const
	{
		return mPipelineLayout->GetSetLayout(set)->GetNumBindings();
	}

	DescriptorSetLayoutBinding* ShaderProgram::GetDescriptorBinding(U32 set, U32 idx)
	{
		return mPipelineLayout->GetSetLayout(set)->GetLayoutBinding(idx);
	}
	DescriptorSetLayout* ShaderProgram::GetDescriptorSetLayout(UniformFrequency freq)
	{
		return mPipelineLayout->GetSetLayout((U32)freq);
	}
	DescriptorSetLayout* ShaderProgram::GetDescriptorSetLayout(U32 set)
	{
		return mPipelineLayout->GetSetLayout(set);
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
		if (mStageMask & (U32)ShaderStage::VS_PS)
		{
			U32 maxSets = 0;
			for (auto& shader : mShaders)
			{
				if (shader)
				{
					auto* reflection = shader->GetReflection();
					maxSets = Max(maxSets, reflection->GetNumDescriptorSetLayouts());
				}
			}
			mPipelineLayout->ResizeLayouts(maxSets);

			for (auto& shader : mShaders)
			{
				if (!shader)
					continue;
				auto* reflection = shader->GetReflection();
				Vector<DescriptorSetLayout>& setLayouts = reflection->GetDescriptorSetLayouts();
				for (U32 setIdx = 0; setIdx < setLayouts.Size(); ++setIdx)
				{
					DescriptorSetLayout* curSetLayout = mPipelineLayout->GetSetLayout(setIdx);
					for (U32 bindingIdx = 0; bindingIdx < setLayouts[setIdx].GetNumBindings(); ++bindingIdx)
					{
						DescriptorSetLayoutBinding* binding = setLayouts[setIdx].GetLayoutBinding(bindingIdx);
						if (!curSetLayout->AddBinding(binding))
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
			auto* graphics = GetSubsystem<Graphics>();
			for (U32 i = 0; i < mPipelineLayout->GetLayoutsSize(); ++i)
				graphics->SetDescriptorSetLayout(mPipelineLayout->GetSetLayout(i));
			graphics->CreateShaderProgram(this);
			graphics->CreatePipelineLayout(mPipelineLayout);
		}
	}

	U32 ShaderProgram::GetNumDescriptorSetLayouts()
	{
		if (mPipelineLayout)
			return mPipelineLayout->GetLayoutsSize();
	}

	void ShaderProgram::SetShader(ShaderStage s, const String& name)
	{
		SetName(name);
		if ((U32)s & (U32)ShaderStage::HS)
		{
			Shader* shader = NEW_OBJECT(Shader);
			shader->SetShader(name, ShaderStage::HS);
			SetShader(ShaderStage::HS, shader);
		}
		if ((U32)s & (U32)ShaderStage::DS)
		{
			Shader* shader = NEW_OBJECT(Shader);
			shader->SetShader(name, ShaderStage::DS);
			SetShader(ShaderStage::DS, shader);
		}
		if ((U32)s & (U32)ShaderStage::GS)
		{
			Shader* shader = NEW_OBJECT(Shader);
			shader->SetShader(name, ShaderStage::GS);
			SetShader(ShaderStage::GS, shader);
		}
		if ((U32)s & (U32)ShaderStage::VS)
		{
			Shader* shader = NEW_OBJECT(Shader);
			shader->SetShader(name, ShaderStage::VS);
			SetShader(ShaderStage::VS, shader);
		}
		if ((U32)s & (U32)ShaderStage::PS)
		{
			Shader* shader = NEW_OBJECT(Shader);
			shader->SetShader(name, ShaderStage::PS);
			SetShader(ShaderStage::PS, shader);
		}
	}
	void ShaderProgram::SetShader(ShaderStage s, const String& vs, const String& ps)
	{
		if (((U32)s & (U32)ShaderStage::VS_PS) == 0)
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

		if (mStageMask == (U32)ShaderStage::VS_HS_DS_PS)
			return 4;

		if (mStageMask == (U32)ShaderStage::CS)
			return 1;
		return 0;
	}

	U32 ShaderProgram::GetUniformMemberAndBinding(U32 set, U32 ID, DescriptorSetLayoutBinding::Member& member)
	{
		if (mPipelineLayout->GetLayoutsSize() > set)
		{
			return mPipelineLayout->GetSetLayout(set)->GetUniformMemberAndBinding(ID, member);
		}
		return U32_MAX;
	}
}