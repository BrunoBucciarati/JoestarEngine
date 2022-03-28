#include "Descriptor.h"
namespace Joestar {
	bool operator==(const DescriptorSetLayoutBinding& lhs, const DescriptorSetLayoutBinding& rhs)
	{
		return lhs.binding == rhs.binding;
	}

	bool DescriptorSetLayout::AddBinding(DescriptorSetLayoutBinding binding)
	{
		auto it = mLayoutBindings.Find(binding);
		if (it == mLayoutBindings.End())
		{
			mLayoutBindings.Push(binding);
			return true;
		}
		return it->MergeBinding(binding);
	}

	U32 DescriptorSetLayoutBinding::Hash()
	{
		U32 hash = 0;
		//随便哈希个，真要冲突了再改2333
		hash += (U32)type;
		hash = hash << 3;
		hash += (U32)stage;
		hash = hash << 3;
		for (auto& id : memberIDs)
		{
			hash += id;
			hash << 3;
		}
		return hash;
	}

	U32 DescriptorSetLayout::Hash()
	{
		U32 hash = 0;
		for (auto binding : mLayoutBindings)
		{
			//随便哈希个，真要冲突了再改2333
			hash += binding.Hash();
			hash = hash << 5;
		}
		return hash;
	}
}