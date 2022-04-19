#pragma once
#include "Shader/ShaderProgram.h"
#include "../Container/Str.h"
#include "../Container/Vector.h"
#include "../Core/Object.h"
#include "Texture2D.h"

namespace Joestar {
	class Graphics;
	class Material : public Object {
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
		void UpdateDescriptorSets();

	private:
		Vector<SharedPtr<Texture>> mTextures;
		Vector<SharedPtr<UniformBuffer>> mUniformBuffers;
		SharedPtr<DescriptorSets> mDescriptorSets;
		SharedPtr<DescriptorSetLayout> mDescriptorSetLayout;
		WeakPtr<Graphics> mGraphics;
	};
}
