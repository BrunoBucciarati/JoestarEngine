#pragma once
#include "GPUResource.h"
#include "../Core/Object.h"
#include "../Container/Ptr.h"
#include "../IO/MemoryManager.h"

namespace Joestar
{
	class Graphics;

	class IndexBuffer : public Object
	{
		REGISTER_OBJECT_ROOT(IndexBuffer);
	public:
		explicit IndexBuffer(EngineContext* ctx);
		U8* GetData()
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
		void SetSize(U32 sz, bool largeIndices = false)
		{
			mIndexCount = sz;
			mIndexSize = largeIndices ? 4 : 2;
			mData = JOJO_NEW_ARRAY(U8, GetSize(), MEMORY_GFX_STRUCT);
			bLargeIndices = largeIndices;
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
		GPUResourceHandle GetGPUHandle()
		{
			return mGPUBuffer->handle;
		}

	private:
		U32 mIndexSize{ 0 };
		U32 mIndexCount{ 0 };
		U8* mData;
		GPUIndexBuffer* mGPUBuffer;
		WeakPtr<Graphics> mGraphics;
		bool bLargeIndices{ false };
		bool bDynamic{ false };
	};
}