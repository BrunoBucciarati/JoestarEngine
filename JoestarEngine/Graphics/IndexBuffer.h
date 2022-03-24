#pragma once
#include "GPUResource.h"
#include "../Core/Object.h"
#include "../Container/Ptr.h"
namespace Joestar
{
	class Graphics;

	class IndexBuffer : Object
	{
		REGISTER_OBJECT_ROOT(IndexBuffer);
	public:
		explicit IndexBuffer(EngineContext* ctx);
		U8* Data()
		{
			return mData;
		}
		void SetGPUBuffer(GPUIndexBuffer* ib)
		{
			mGPUBuffer = ib;
		}
		U32 GetSize()
		{
			return mIndexSize * mIndexCount;
		}
		U32 GetIndexSize() const
		{
			return mIndexSize;
		}
		U32 GetIndexCount() const
		{
			return mIndexCount;
		}
		void SetData(U8* data);

	private:
		U32 mIndexSize{ 0 };
		U32 mIndexCount{ 0 };
		U8* mData;
		GPUIndexBuffer* mGPUBuffer;
		WeakPtr<Graphics> mGraphics;
		bool bDynamic{ false };
	};
}