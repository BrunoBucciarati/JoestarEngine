#pragma once
#include "../../Core/Minimal.h"
#include "Shader.h"
#include "../Descriptor.h"
namespace Joestar {
	class ShaderProgram : public Object
	{
		REGISTER_OBJECT_ROOT(ShaderProgram);
	public:
		explicit ShaderProgram(EngineContext* ctx);
		void SetShader(ShaderStage stage, Shader* vs, Shader* ps);
		void SetShader(ShaderStage stage, const String& vs, const String& ps);
		void SetShader(ShaderStage stage, const String& name);
		void SetShader(ShaderStage stage, Shader* shader);
		Shader* GetShader(ShaderStage stage);
		Shader* GetShader(U32 index);
		GPUResourceHandle GetHandle()
		{
			return mGPUHandle;
		}
		void SetHandle(GPUResourceHandle handle)
		{
			mGPUHandle = handle;
		}
		U32 GetNumStages();
		U32 GetStageMask()
		{
			return mStageMask;
		}
		U32 GetNumDescriptorSetLayouts() const
		{
			return mDescriptorLayouts.Size();
		}
		U32 GetNumDescriptorBindings(U32 set) const
		{
			return mDescriptorLayouts[set].GetNumBindings();
		}
		DescriptorSetLayoutBinding& GetDescriptorBinding(U32 set, U32 idx)
		{
			return mDescriptorLayouts[set].GetLayoutBinding(idx);
		}
		DescriptorSetLayout& GetDescriptorSetLayout(U32 set)
		{
			return mDescriptorLayouts[set];
		}
		PODVector<InputAttribute>& GetInputAttributes()
		{
			return mInputAttributes;
		}
		DescriptorSetLayoutBinding GetUniformBinding(UniformFrequency set, U32 ID)
		{
			return GetUniformBinding(U32(set), ID);
		}
		DescriptorSetLayoutBinding GetUniformBinding(U32 set, U32 ID);
	private:
		void CheckValid();
		void CollectInputAndDescriptors();
		Vector<SharedPtr<Shader>> mShaders;
		Vector<DescriptorSetLayout> mDescriptorLayouts;
		PODVector<InputAttribute> mInputAttributes;
		U32 mStageMask{ 0 };
		bool bValid{ false };
		GPUResourceHandle mGPUHandle;
	};
}