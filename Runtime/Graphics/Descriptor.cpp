#include "Descriptor.h"
namespace Joestar {
	bool operator==(const DescriptorSetLayoutBinding& lhs, const DescriptorSetLayoutBinding& rhs)
	{
		return lhs.binding == rhs.binding;
	}

	bool DescriptorSetLayout::AddBinding(DescriptorSetLayoutBinding* rhs)
	{
		for (auto& binding : mLayoutBindings)
		{
			if (binding->binding == rhs->binding)
			{
				return binding->MergeBinding(rhs);
			}
		}
		mLayoutBindings.Push(SharedPtr<DescriptorSetLayoutBinding>(rhs));
		return true;
	}

	U32 DescriptorSetLayoutBinding::Hash()
	{
		U32 hash = 0;
		//随便哈希个，真要冲突了再改2333
		hash += (U32)type;
		hash = hash << 3;
		hash += (U32)stage;
		hash = hash << 3;
		for (auto& member : members)
		{
			hash += member.ID;
			hash << 3;
		}
		return hash;
	}


	U32 DescriptorSetLayout::GetUniformMemberAndBinding(U32 ID, DescriptorSetLayoutBinding::Member& member)
	{
		for (U32 i = 0; i < mLayoutBindings.Size(); ++i)
		{
			auto& layoutBinding = mLayoutBindings[i];
			for (U32 memberIdx = 0; memberIdx < layoutBinding->members.Size(); ++memberIdx)
			{
				if (layoutBinding->members[memberIdx].ID == ID)
				{
					member = layoutBinding->members[memberIdx];
					return layoutBinding->binding;
				}
			}
		}
		return U32_MAX;
	}

	U32 DescriptorSetLayout::Hash()
	{
		U32 hash = 0;
		for (auto binding : mLayoutBindings)
		{
			//随便哈希个，真要冲突了再改2333
			hash += binding->Hash();
			hash = hash << 5;
		}
		return hash;
	}

	void DescriptorSets::AllocFromLayout(DescriptorSetLayout* layout)
	{
		mSize = 0;
		mSets.Resize(layout->GetNumBindings());
		for (U32 i = 0; i < layout->GetNumBindings(); ++i)
		{
			auto* binding = layout->GetLayoutBinding(i);
			mSize += binding->size;
			mSets[i].binding = binding->binding;
			mSets[i].type = binding->type;
			mSets[i].count = binding->count;
		}
		mBuffer = JOJO_NEW_ARRAY(U8, mSize);
		mLayout = layout;
	}

	U32 DescriptorSets::GetDescriptorSetBinding(U32 ID)
	{
		for (U32 i = 0; i < mLayout->GetNumBindings(); ++i)
		{
			auto* binding = mLayout->GetLayoutBinding(i);
			auto& set = mSets[i];
			for (U32 j = 0; j < binding->members.Size(); ++j)
			{
				if (ID == binding->members[j].ID)
				{
					return i;
				}
			}
		}
		return U32_MAX;
	}

	void DescriptorSets::SetLayoutData(U32 ID, float* data)
	{
		U32 offset = 0;
		for (U32 i = 0; i < mLayout->GetNumBindings(); ++i)
		{
			auto* binding = mLayout->GetLayoutBinding(i);
			auto& set = mSets[i];
			for (U32 j = 0; j < binding->members.Size(); ++j)
			{
				if (ID == binding->members[j].ID)
				{
					offset += binding->members[j].offset;
					memcpy(mBuffer + offset, data, binding->members[j].size);
				}
			}
			offset += binding->size;
		}
	}
}