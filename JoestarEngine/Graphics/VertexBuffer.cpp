#include "VertexBuffer.h"
#include "Graphics.h"

namespace Joestar
{
	VertexBuffer::VertexBuffer(EngineContext* ctx) : Super(ctx),
		mGraphics(GetSubsystem<Graphics>()),
		mGPUBuffer(nullptr)
	{
	}

	void VertexBuffer::SetData(U8* data)
	{
		mData = data;
		mGPUBuffer = mGraphics->CreateGPUVertexBuffer(this);
	}
}