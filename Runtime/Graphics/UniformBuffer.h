#pragma once
#include "GPUResource.h"
#include "../Core/Minimal.h"
namespace Joestar
{
	class DescriptorSetLayoutBinding;
	class Graphics;

	class UniformBuffer : public GPUResource
	{
		GET_SET_STATEMENT_INITVALUE(U32, Binding, 0);
	public:
		void AllocFromBinding(DescriptorSetLayoutBinding* binding);
		void SetHash(U32 hash)
		{
			mHash = hash;
		}
		UniformFrequency GetFrequency() const
		{
			return mFrequency;
		}
		U32 GetSet()
		{
			return (U32)mFrequency;
		}
		void SetFrequency(UniformFrequency freq)
		{
			mFrequency = freq;
		}
		void SetSize(U32 sz);
		U32 GetSize() const
		{
			return mSize;
		}
		void SetData(U32 offset, U32 sz, U8* data);
		U8* GetBuffer()
		{
			return mBuffer;
		}
	private:
		U32 mSize{ 0 };
		U8* mBuffer{ nullptr };
		UniformFrequency mFrequency;
		WeakPtr<Graphics> mGraphics;
	};
}