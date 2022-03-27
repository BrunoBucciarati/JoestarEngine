#pragma once
#include "../Container/Vector.h"
#include "GraphicDefines.h"
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

	struct DescriptorSetLayoutBinding
	{
		U32 binding{ 0 };
		DescriptorType type;
		U32 count{ 0 };
		U32 stage{ 0 };
		String name;
		bool operator==(const DescriptorSetLayoutBinding& rhs)
		{
			return binding == rhs.binding;
		}
		bool MergeBinding(const DescriptorSetLayoutBinding& rhs)
		{
			if (rhs.type != type)
				return false;
			stage |= rhs.stage;
			return true;
		}
		friend bool operator==(const DescriptorSetLayoutBinding& lhs, const DescriptorSetLayoutBinding& rhs);
	};
	bool operator==(const DescriptorSetLayoutBinding& lhs, const DescriptorSetLayoutBinding& rhs);

	class DescriptorSetLayout
	{
	public:
		DescriptorSetLayoutBinding& GetLayoutBindings(U32 idx)
		{
			return mLayoutBindings[idx];
		}
		void SetNumBindings(U32 num)
		{
			mLayoutBindings.Resize(num);
		}
		U32 GetNumBindings() const
		{
			return mLayoutBindings.Size();
		}
		bool AddBinding(const DescriptorSetLayoutBinding& binding);
	private:
		Vector<DescriptorSetLayoutBinding> mLayoutBindings;
	};

	struct VertexInputAttribute
	{
		U32 location{ 0 };
		U32 binding{ 0 };
		VertexType type;
		U32 offset{ 0 };
	};
}