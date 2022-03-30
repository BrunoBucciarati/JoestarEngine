#include "UniformBuffer.h"
#include "../Core/Minimal.h"

namespace Joestar {
	void UniformBuffer::SetSize(U32 sz)
	{
		mSize = sz;
		if (mBuffer)
			JOJO_DELETE_ARRAY(mBuffer);
		mBuffer = JOJO_NEW_ARRAY(U8, sz);
	}

	void UniformBuffer::SetData(U32 offset, U32 sz, U8* data)
	{
		memcpy(mBuffer + offset, data, sz);
	}
}