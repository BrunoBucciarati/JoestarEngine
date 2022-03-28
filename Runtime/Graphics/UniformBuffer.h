#pragma once
#include "GPUResource.h"
namespace Joestar
{
	class UniformBuffer : public GPUResource
	{
		GET_SET_STATEMENT(UniformType, Type);
		GET_SET_STATEMENT(String, Name);
		GET_SET_STATEMENT_INITVALUE(U32, Binding, 0);
		GET_SET_STATEMENT_INITVALUE(U32, Set, 0);
		GET_SET_STATEMENT_INITVALUE(U32, ID, 0);
	public:
		void SetName(const char* name)
		{
			mName = name;
		}

		U32 GetDataSize() const
		{
			return UniformDataTypeSize[(U32)mType.dataType];
		}
		UniformFrequency GetFrequency() const
		{
			return mType.frequency;
		}
	};
}