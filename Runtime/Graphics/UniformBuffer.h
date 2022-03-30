#pragma once
#include "GPUResource.h"
#include "../Core/ObjectDefines.h"
namespace Joestar
{
	class UniformBuffer : public GPUResource
	{
		GET_SET_STATEMENT_INITVALUE(U32, Binding, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Set, 0);
	public:
		void SetHash(U32 hash)
		{
			mHash = hash;
		}
		UniformFrequency GetFrequency() const
		{
			return mFrequency;
		}

		//UniformType GetType()
		//{
		//	return mType;
		//}
		//void SetType(UniformType& type)
		//{
		//	mType = type;
		//}
		void SetSize(U32 sz);
		U32 GetSize() const
		{
			return mSize;
		}
		void SetData(U32 offset, U32 sz, U8* data);
	private:
		U32 mSize{ 0 };
		U8* mBuffer{ nullptr };
		UniformType mType;
		UniformFrequency mFrequency;
	};
}