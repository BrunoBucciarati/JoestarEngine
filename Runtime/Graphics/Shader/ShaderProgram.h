#pragma once
#include "../../Core/Minimal.h"
#include "Shader.h"
namespace Joestar {
	class ShaderProgram : public Object
	{
		REGISTER_OBJECT_ROOT(ShaderProgram);
	public:
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
	private:
		Vector<SharedPtr<Shader>> mShaders;
		U32 mStageMask;
		bool bValid;
		GPUResourceHandle mGPUHandle;
	};
}