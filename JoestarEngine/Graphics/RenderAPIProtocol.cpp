#include "RenderAPIProtocol.h"
#include "Window.h"
namespace Joestar {
	void RenderAPIProtocol::SetWindow(Window* wd)
	{
		window = wd;
	}

	void RenderAPIProtocol::CreateMemory(GPUResourceHandle handle, U32 size, U8* data)
	{
		if (handle + 1 > mMemories.Size())
			mMemories.Resize(handle + 1);
		mMemories[handle].size = size;
		mMemories[handle].data = data;
	}
}