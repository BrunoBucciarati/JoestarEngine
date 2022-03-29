#pragma once
#include "../Core/Minimal.h"
#include "../Component/Component.h"
#include "../Graphics/PipelineState.h"
#include "../Graphics/CommandBuffer.h"
#include "../Graphics/Material.h"
namespace Joestar {
	class Graphics;
	class GraphicsPipelineState;
	class Renderer : public Component {
		REGISTER_COMPONENT_ROOT(Renderer);
	public:
		void Init();
		virtual void Render(CommandBuffer*) = 0;
		void RenderToShadowMap();
		virtual SharedPtr<GraphicsPipelineState> GetPipelineState(CommandBuffer* cb);
		void SetMaterial(Material* mat)
		{
			mMaterial = mat;
		}

		void SetShader(const String& name, ShaderStage stage = ShaderStage::VS_PS);

		void SetShader(Shader* shader, ShaderStage stage = ShaderStage::VS_PS)
		{
			mShaderProgram->SetShader(stage, shader);
		}
		Shader* GetShader(ShaderStage stage)
		{
			return mShaderProgram->GetShader(stage);
		}
		String GetShaderName(ShaderStage stage)
		{
			return GetShader(stage)->GetName();
		}
		Material* GetMaterial()
		{
			return mMaterial;
		}
		ShaderProgram* GetShaderProgram()
		{
			return mShaderProgram;
		}
		void SetUniformBuffer(PerObjectUniforms,float*);
		SharedPtr<DescriptorSets>& GetDescriptorSets()
		{
			return mDescriptorSets;
		}
	protected:
		WeakPtr<Graphics> mGraphics;
		Vector<SharedPtr<GraphicsPipelineState>> mPSOs;
		SharedPtr<Material> mMaterial;
		SharedPtr<ShaderProgram> mShaderProgram;
		SharedPtr<DescriptorSets> mDescriptorSets;
	};
}