#pragma once
#include "GPUResource.h"
#include "../Core/Object.h"
#include "../Container/Ptr.h"
#include "../Container/Vector.h"
#include "../IO/MemoryManager.h"
namespace Joestar
{
	class Graphics;

	struct VertexElement {
		VertexSemantic semantic;
		VertexType type;
	};

	class VertexBuffer : Object
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
		void SetSize(U32 sz, PODVector<VertexElement>& elements)
		{
			mVertexElements = elements;
			mData = JOJO_NEW_ARRAY(U8, sz, MEMORY_GFX_STRUCT);
		}
		U32 GetVertexSize() const
		{
			return mVertexSize;
		}
		U32 GetVertexCount() const
		{
			return mVertexCount;
		}
		void SetData(U8* data);

		

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