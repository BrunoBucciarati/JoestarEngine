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

	U32 DescriptorSetLayout::Hash()
	{
		U32 hash = 0;
		for (auto binding : mLayoutBindings)
		{
			//随便哈希个，真要冲突了再改2333
			hash += (U32)binding.type;
			hash = hash << 5;
			hash += (U32)binding.stage;
			hash = hash << 5;
		}
		return hash;
	}
}