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
}