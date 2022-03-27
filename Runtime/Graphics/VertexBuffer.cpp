#include "VertexBuffer.h"
#include "Graphics.h"

namespace Joestar
{
	VertexBuffer::VertexBuffer(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>()),
		mGPUBuffer(nullptr)
	{
	}
	VertexBuffer::~VertexBuffer()
	{
		mGraphics->RemoveGPUVertexBuffer(mGPUBuffer);
	}

	void VertexBuffer::SetSize(U32 count, PODVector<VertexElement>&elements)
	{
		SetVertexElements(elements);
		mVertexCount = count;
		mData = JOJO_NEW_ARRAY(U8, GetSize(), MEMORY_GFX_STRUCT);
	}

	void VertexBuffer::SetData(U8* data)
	{
		memcpy(mData, data, GetSize());
		mGPUBuffer = mGraphics->CreateGPUVertexBuffer(this);
	}

	void VertexBuffer::SetVertexElements(PODVector<VertexElement>& elements)
	{
		mVertexElements = elements;
		mVertexSize = 0;
		for (auto& ele : elements)
		{
			mVertexSize += VertexTypeSize[(U32)ele.type];
		}
	}

	U32 VertexBuffer::GetElementOffset(VertexSemantic sem)
	{
		U32 offset = 0;
		for (auto& element : mVertexElements)
		{
			if (element.semantic == sem)
			{
				return offset;
			}
			offset += VertexTypeSize[(U32)element.type];
		}
	}
}