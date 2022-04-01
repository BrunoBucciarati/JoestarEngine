#pragma once
#include "../../Core/Minimal.h"
#include "Shader.h"
#include "../Descriptor.h"
namespace Joestar {
	class PipelineLayout;
	class Graphics;
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
		U32 GetNumDescriptorSetLayouts();

		U32 GetNumDescriptorBindings(U32 set) const;
		
		DescriptorSetLayoutBinding* GetDescriptorBinding(U32 set, U32 idx);
		DescriptorSetLayout* GetDescriptorSetLayout(UniformFrequency freq);
		DescriptorSetLayout* GetDescriptorSetLayout(U32 set);
		PODVector<InputAttribute>& GetInputAttributes()
		{
			return mInputAttributes;
		}
		U32 GetUniformMemberAndBinding(UniformFrequency set, U32 ID, DescriptorSetLayoutBinding::Member& binding)
		{
			return GetUniformMemberAndBinding(U32(set), ID, binding);
		}
		U32 GetUniformMemberAndBinding(U32 set, U32 ID, DescriptorSetLayoutBinding::Member& binding);
		bool IsValid() const
		{
			return bValid;
		}
		PipelineLayout* GetPipelineLayout()
		{
			return mPipelineLayout;
		}
		SharedPtr<DescriptorSets> GetDescriptorSets(UniformFrequency freq)
		{
			return mAllDescriptorSets[(U32)freq];
		}
		Vector<SharedPtr<DescriptorSets>>& GetAllDescriptorSets()
		{
			return mAllDescriptorSets;
		}
	private:
		void CheckValid();
		void CollectInputAndDescriptors();
		void AllocDescriptorSets();
		Vector<SharedPtr<Shader>> mShaders;
		PODVector<InputAttribute> mInputAttributes;
		U32 mStageMask{ 0 };
		bool bValid{ false };
		GPUResourceHandle mGPUHandle;
		SharedPtr<PipelineLayout> mPipelineLayout;
		Vector<SharedPtr<DescriptorSets>> mAllDescriptorSets;
		WeakPtr<Graphics> mGraphics;
	};
}