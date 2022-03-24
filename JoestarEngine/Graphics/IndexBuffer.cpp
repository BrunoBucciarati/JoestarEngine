#include "IndexBuffer.h"
#include "Graphics.h"

namespace Joestar
{
	IndexBuffer::IndexBuffer(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>()),
		mGPUBuffer(nullptr)
	{
	}

	void IndexBuffer::SetData(U8* data)
	{
		mData = data;
		mGPUBuffer = mGraphics->CreateGPUIndexBuffer(this);
	}
}