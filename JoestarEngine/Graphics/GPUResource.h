#pragma once
#include "../Core/RefCount.h"
#include "GraphicDefines.h"
namespace Joestar
{
	typedef U32 GPUResourceHandle;

	class GPUResourceCreateInfo
	{
	public:
		MemoryReadWriter rwMem;
		bool Empty() const
		{
			return rwMem.Empty();
		}
	};

	class GPUResource : public RefCount
	{
	public:
		GPUResourceHandle handle;
		GPUResourceCreateInfo* createInfo;
	};

	class GPUMemory : public GPUResource
	{
	public:
		U32 size;
		U8* data;
	};

	class GPUVertexBuffer : public GPUResource
	{
	public:
		GPUMemory* memory;
	};

	class GPUIndexBuffer : public GPUResource
	{
	public:
		GPUMemory* memory;
	};

	class GPUUniformBuffer : public GPUResource
	{
		U32 hash;
	};
}