#pragma once
#include "../Core/Platform.h"
#include "../IO/MemoryWriter.h"
namespace Joestar
{
	typedef U32 GPUResourceHandle;
	class GPUResource
	{
	public:
		GPUResourceHandle handle;
	};



	class GPUResourceCreateInfo
	{
	public:
		MemoryReader reader;
		bool Empty() const
		{
			return reader.Empty();
		}
	};
}