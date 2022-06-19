#pragma once
#include "../Container/Vector.h"
#include "../Container/Ptr.h"
#include "GraphicDefines.h"
#include "GPUResource.h"
#include "UniformBuffer.h"
#include "Texture.h"
namespace Joestar
{
	enum class DescriptorType
	{
		SAMPLER = 0,
		COMBINED_IMAGE_SAMPLER = 1,
		SAMPLED_IMAGE = 2,
		STORAGE_IMAGE = 3,
		UNIFORM_TEXEL_BUFFER = 4,
		STORAGE_TEXEL_BUFFER = 5,
		UNIFORM_BUFFER = 6,
		STORAGE_BUFFER = 7,
		UNIFORM_BUFFER_DYNAMIC = 8,
		STORAGE_BUFFER_DYNAMIC = 9,
		INPUT_ATTACHMENT = 10,
	};

	class DescriptorSetLayoutBinding : public Hashable, public RefCount
	{
	public:
		U32 binding{ 0 };
		DescriptorType type;
		U32 count{ 0 };
		U32 stage{ 0 };
		U32 size{ 0 };
		struct Member
		{
			U32 ID;
			U32 offset;
			U32 size;
		};
		PODVector<Member> members;
		bool operator==(const DescriptorSetLayoutBinding& rhs)
		{
			return binding == rhs.binding;
		}
		bool MergeBinding(const DescriptorSetLayoutBinding* rhs)
		{
			if (rhs->type != type)
				return false;
			stage |= rhs->stage;
			return true;
		}
		friend bool operator==(const DescriptorSetLayoutBinding& lhs, const DescriptorSetLayoutBinding& rhs);
		U32 Hash();
		DescriptorSetLayoutBinding& operator=(const DescriptorSetLayoutBinding& rhs);
	};
	bool operator==(const DescriptorSetLayoutBinding& lhs, const DescriptorSetLayoutBinding& rhs);

	class DescriptorSetLayout : public GPUResource
	{
	public:
		DescriptorSetLayoutBinding* GetLayoutBinding(U32 idx)
		{
			return mLayoutBindings[idx];
		}
		void SetNumBindings(U32 num)
		{
			mLayoutBindings.Resize(num);
			for (U32 i = 0; i < num; ++i)
			{
				mLayoutBindings[i] = JOJO_NEW(DescriptorSetLayoutBinding, MEMORY_GFX_STRUCT);
			}
		}
		U32 GetNumBindings() const
		{
			return mLayoutBindings.Size();
		}
		bool AddBinding(DescriptorSetLayoutBinding* binding);
		U32 GetUniformMemberAndBinding(U32 ID, DescriptorSetLayoutBinding::Member& member);

		U32 Hash();
	private:
		Vector<SharedPtr<DescriptorSetLayoutBinding>> mLayoutBindings;
	};

	struct DescriptorSet
	{
		U32 binding{ 0 };
		U32 offset{ 0 };
		U32 set{ 0 };
		U32 count{ 0 };
		U32 size{ 0 };
		DescriptorType type;
		SharedPtr<UniformBuffer> ub;
		SharedPtr<Texture> texture;
	};

	class DescriptorSets : public GPUResource
	{
	public:
		void AllocFromLayout(DescriptorSetLayout* layout);
		DescriptorSet& GetDescriptorSetByBinding(U32 binding);
		void SetBindingUniformBuffer(U32 binding, UniformBuffer* ub);
		void SetBindingTexture(U32 binding, Texture* ub);
		DescriptorSet& GetDescriptorSetByID(U32 ID);
		void SetLayoutData(U32 ID, float* data);
		SharedPtr<DescriptorSetLayout>& GetLayout()
		{
			return mLayout;
		}
		U32 Size()
		{
			return mSets.Size();
		}
		U8* GetBuffer()
		{
			return mBuffer;
		}
		DescriptorSet& GetDescriptorSet(U32 idx)
		{
			return mSets[idx];
		}
	private:
		Vector<DescriptorSet> mSets;
		SharedPtr<DescriptorSetLayout> mLayout;
		U32 mSize;
		U8* mBuffer;
	};

	struct InputAttribute {
		U32    location;
		U32    binding;
		VertexType    format;
		U32    offset;
		String name;
		U32 Hash()
		{
			return ((location * 31 + binding) * 31 + (U32)format) * 31 + offset;
		}
	};

	struct InputBinding
	{
		U32 binding{ 0 };
		U32 stride{ 0 };
		bool instance{ false };
		U32 Hash()
		{
			return (binding * 31 + stride) * 31 + instance ? 1 : 0;
		}
	};
}