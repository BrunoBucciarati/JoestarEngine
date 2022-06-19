#pragma once
#include "../Core/Minimal.h"
#include "../Core/Hashable.h"
#include "../Graphics/GraphicDefines.h"
namespace Joestar
{
	class Material;
	class DescriptorSets;
	class UniformBuffer;
	class Texture;
	class DepthStencilState;
	class GraphicsPipelineState;
	class Graphics;
	class ShaderProgram;
	class MaterialInstance : public Object, public Hashable
	{
		REGISTER_OBJECT(MaterialInstance, Object);
	public:
		explicit MaterialInstance(EngineContext* ctx, Material* mat);
		void SetMaterial(Material* mat)
		{
			mMaterial = mat;
		}
		Material* GetMaterial()
		{
			return mMaterial;
		}
		bool Update();
		void SetTexture(Texture* tex, U32 binding = 0);
		void SetDepthCompareOp(CompareOp op);
		SharedPtr<DepthStencilState> GetDepthStencilState() const;
		DescriptorSets* GetBatchDescriptorSets()
		{
			return mBatchDescriptorSets;
		}
		DescriptorSets* GetObjectDescriptorSets()
		{
			return mObjectDescriptorSets;
		}

		void SetUniformBuffer(PerObjectUniforms uniform, U8* data);
		void SetUniformBuffer(PerBatchUniforms uniform, U8* data);
		void UpdateDescriptorSets();

		GraphicsPipelineState* GetPipelineState()
		{
			return mPiplelineState;
		}

		ShaderProgram* GetShaderProgram();

	private:
		void Init();
		Vector<SharedPtr<Texture>> mTextures;
		SharedPtr<Material> mMaterial;
		WeakPtr<Graphics> mGraphics;
		SharedPtr<DescriptorSets> mBatchDescriptorSets;
		SharedPtr<DescriptorSets> mObjectDescriptorSets;
		Vector<SharedPtr<UniformBuffer>> mBatchUniformBuffers;
		Vector<SharedPtr<UniformBuffer>> mObjectUniformBuffers;
		SharedPtr<DepthStencilState> mDepthStencilState;
		SharedPtr<GraphicsPipelineState> mPiplelineState;
		bool bDepthStencilStateDirty{ false };
	};
}