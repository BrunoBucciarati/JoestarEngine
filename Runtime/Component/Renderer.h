#pragma once
#include "../Core/Minimal.h"
#include "../Component/Component.h"
#include "../Graphics/PipelineState.h"
#include "../Graphics/CommandBuffer.h"
#include "../Graphics/MaterialInstance.h"
#include "../Math/AABB.h"
namespace Joestar {
	class Graphics;
	class GraphicsPipelineState;
	class Renderer : public Component {
		REGISTER_COMPONENT_ROOT(Renderer);
	public:
		void Init();
		virtual void Render(CommandBuffer*) = 0;
		virtual SharedPtr<GraphicsPipelineState> GetPipelineState(CommandBuffer* cb);
		void SetMaterial(MaterialInstance* mat)
		{
			mMaterial = mat;
		}

		void SetMaterial(const String& vs, const String& ps);
		void SetMaterial(const String& vsps);

		MaterialInstance* GetMaterial()
		{
			return mMaterial;
		}

		//void SetShader(const String& name, ShaderStage stage = ShaderStage::VS_PS);

		//void SetShader(Shader* shader, ShaderStage stage = ShaderStage::VS_PS)
		//{
		//	mShaderProgram->SetShader(stage, shader);
		//}
		//Shader* GetShader(ShaderStage stage)
		//{
		//	return mShaderProgram->GetShader(stage);
		//}
		//String GetShaderName(ShaderStage stage)
		//{
		//	return GetShader(stage)->GetName();
		//}
		//ShaderProgram* GetShaderProgram()
		//{
		//	return mShaderProgram;
		//}
		//void SetUniformBuffer(PerObjectUniforms,U8*);
		//SharedPtr<DescriptorSets>& GetDescriptorSets()
		//{
		//	return mDescriptorSets;
		//}
	protected:
		WeakPtr<Graphics> mGraphics;
		Vector<SharedPtr<GraphicsPipelineState>> mPSOs;
		SharedPtr<MaterialInstance> mMaterial;
		//SharedPtr<ShaderProgram> mShaderProgram;
		//SharedPtr<DescriptorSets> mDescriptorSets;
		//Vector<SharedPtr<UniformBuffer>> mUniformBuffers;
	};
}