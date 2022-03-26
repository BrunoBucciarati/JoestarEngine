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
	};

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