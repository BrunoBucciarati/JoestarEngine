#pragma once
#include "GPUResource.h"
#include "../Core/Object.h"
#include "../Container/Ptr.h"
#include "../Container/Vector.h"
#include "../IO/MemoryManager.h"
namespace Joestar
{
	class Graphics;

	class VertexBuffer : public Object
	{
		REGISTER_OBJECT_ROOT(VertexBuffer);
	public:
		explicit VertexBuffer(EngineContext* ctx);
		U8* GetData()
		{
			return mData;
		}
		void SetGPUBuffer(GPUVertexBuffer* vb)
		{
			mGPUBuffer = vb;
		}
		U32 GetSize() const
		{
			return mVertexSize * mVertexCount;
		}
		void SetSize(U32 count, PODVector<VertexElement>& elements);
		
		U32 GetVertexSize() const
		{
			return mVertexSize;
		}
		U32 GetVertexCount() const
		{
			return mVertexCount;
		}
		void SetData(U8* data);

		void SetVertexElements(PODVector<VertexElement>& elements);
		PODVector<VertexElement>& GetVertexElements()
		{
			return mVertexElements;
		}
		GPUResourceHandle GetGPUHandle()
		{
			return mGPUBuffer->handle;
		}
		U32 GetElementOffset(VertexSemantic);

	private:
		U32 mVertexSize{ 0 };
		U32 mVertexCount{ 0 };
		U8* mData;
		GPUVertexBuffer* mGPUBuffer;
		WeakPtr<Graphics> mGraphics;
		bool bDynamic{ false };
		PODVector<VertexElement> mVertexElements;
	};
}