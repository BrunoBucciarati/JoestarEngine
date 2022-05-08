#pragma once
#include "Shader/ShaderProgram.h"
#include "../Container/Str.h"
#include "../Container/Vector.h"
#include "../Container/Ptr.h"
#include "../Core/Object.h"
#include "Texture2D.h"

namespace Joestar {
	class Graphics;
	class DepthStencilState;
	class Material : public Object, public Hashable {
		REGISTER_OBJECT(Material, Object);
	public:
		explicit Material(EngineContext* ctx);
		void SetDefault();
		void SetPBR();
		void SetPlaneMat();

		void SetTexture(Texture* tex, U32 binding = 0);
		Vector<SharedPtr<Texture>>& GetTextures()
		{
			return mTextures;
		}
		Texture* GetTexture(U32 slot)
		{
			return mTextures[slot];
		}

		void SetUniformBuffer(PerBatchUniforms, float* data);
		SharedPtr<DescriptorSets>& GetDescriptorSets()
		{
			return mDescriptorSets;
		}
		void SetDescriptorSetLayout(DescriptorSetLayout* setLayout)
		{
			mDescriptorSetLayout = setLayout;
		}
		void AllocDescriptorSets();
		bool Update();
		void ReHash();
		void SetDepthCompareOp(CompareOp op);
		SharedPtr<DepthStencilState> GetDepthStencilState() const;

	private:
		Vector<SharedPtr<Texture>> mTextures;
		Vector<SharedPtr<UniformBuffer>> mUniformBuffers;
		SharedPtr<DescriptorSets> mDescriptorSets;
		SharedPtr<DescriptorSetLayout> mDescriptorSetLayout;
		WeakPtr<Graphics> mGraphics;
		SharedPtr<DepthStencilState> mDepthStencilState;
		bool bDepthStencilStateDirty{ false };
	};
}
