#pragma once
#include "../Core/Platform.h"
#include "../IO/MemoryWriter.h"
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

	class GPUResource
	{
	public:
		GPUResourceHandle handle;
		GPUResourceCreateInfo* createInfo;
	};
}