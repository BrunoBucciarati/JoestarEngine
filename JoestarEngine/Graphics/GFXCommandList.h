#pragma once
#include "../IO/MemoryWriter.h"
#include <atomic>
namespace Joestar
{
	enum class GFXCommand
	{
		CreateSwapChain = 0,
		CreateCommandBuffer = 1,
		CreateFrameBuffer = 2,
		CreateImage = 3,
		CreateImageView = 4,
		CreateSyncObjects = 5,
		CommandCount
	};
	class GFXCommandList : public MemoryReadWriter
	{
	public:
		void WriteCommand(GFXCommand t)
		{
			WriteBuffer<GFXCommand>(t);
		}

		bool ReadCommand(GFXCommand& t)
		{
			return ReadBuffer<GFXCommand>(t);
		}

		void WriteGPUResourceCreateInfo(GPUResourceCreateInfo& info)
		{
			const U32& sz = info.rwMem.Size();
			WriteBuffer<U32>(sz);

			const U32& cursor = info.rwMem.GetCursor();
			WriteBuffer<U32>(cursor);

			const U32& last = info.rwMem.GetLast();
			WriteBuffer<U32>(last);

			U8* data = info.rwMem.Data();
			WriteBufferPtr(data, sz);
		}

		bool ReadGPUResourceCreateInfo(GPUResourceCreateInfo& info)
		{
			if (!ReadBuffer(info.rwMem.GetSize()))
			{
				return false;
			}

			if (!ReadBuffer(info.rwMem.GetCursor()))
			{
				return false;
			}

			if (!ReadBuffer(info.rwMem.GetLast()))
			{
				return false;
			}

			if (!ReadBufferPtr(info.rwMem.Data(), info.rwMem.GetSize()))
			{
				return false;
			}
			return true;
		}

		void Flush()
		{
			MemoryReadWriter::Flush();
			readFlag = true;
		}
		//当这个标志为true时意味着可读
		std::atomic_bool readFlag{ false };
	};
}