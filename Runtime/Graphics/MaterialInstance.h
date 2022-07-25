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
	class RasterizationState;
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
		void SetTessellationControlPoints(U32);
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

		U32 GetTessellationControlPoints() const
		{
			return mTessellationControlPoints;
		}

		void SetPolygonMode(PolygonMode mode);
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
		SharedPtr<RasterizationState> mRasterizationState;
		SharedPtr<GraphicsPipelineState> mPiplelineState;
		U32 mTessellationControlPoints{ 0 };
		bool bDepthStencilStateDirty{ false };
		bool bRasterStateDirty{ false };
	};
}