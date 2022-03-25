#include "IndexBuffer.h"
#include "Graphics.h"

namespace Joestar
{
	IndexBuffer::IndexBuffer(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>()),
		mGPUBuffer(nullptr)
	{
	}
	IndexBuffer::~IndexBuffer()
	{
		mGraphics->RemoveGPUIndexBuffer(mGPUBuffer);
	}

	void IndexBuffer::SetData(U8* data)
	{
		memcpy(mData, data, GetSize());
		mGPUBuffer = mGraphics->CreateGPUIndexBuffer(this);
	}
}