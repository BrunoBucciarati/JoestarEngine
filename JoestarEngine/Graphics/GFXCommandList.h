#pragma once
#include "../IO/MemoryWriter.h"
#include <atomic>
namespace Joestar
{
	enum class GFXCommand
	{
		CreateCommandBuffer = 0,
		CreateSwapChain
	};
	class GFXCommandList : public MemoryReadWriter
	{
	public:
		void WriteCommand(GFXCommand t) {
			WriteBuffer<GFXCommand>(t);
		}

		bool ReadCommand(GFXCommand& t) {
			return ReadBuffer<GFXCommand>(t);
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