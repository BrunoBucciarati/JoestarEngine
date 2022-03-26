#include "RenderAPIProtocol.h"
#include "Window.h"

#define GET_STRUCT_BY_HANDLE_FROM_VECTOR(_VAR, _TYP, _HANDLE, _VEC) \
    if (_HANDLE + 1 > _VEC.Size()) _VEC.Resize(_HANDLE + 1); \
    _TYP##& _VAR =  _VEC[_HANDLE];

#define GET_STRUCT_BY_HANDLE(_VAR, _TYP, _HANDLE) \
    GET_STRUCT_BY_HANDLE_FROM_VECTOR(_VAR, _TYP, _HANDLE, m##_TYP##s);

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

	void RenderAPIProtocol::CreateColorBlendState(GPUResourceHandle handle, GPUColorBlendStateCreateInfo& createInfo)
	{
		GET_STRUCT_BY_HANDLE_FROM_VECTOR(state, GPUColorBlendStateCreateInfo, handle, mColorBlendStates);
		state = createInfo;
	}
	void RenderAPIProtocol::CreateDepthStencilState(GPUResourceHandle handle, GPUDepthStencilStateCreateInfo& createInfo)
	{
		GET_STRUCT_BY_HANDLE_FROM_VECTOR(state, GPUDepthStencilStateCreateInfo, handle, mDepthStencilStates);
		state = createInfo;
	}
	void RenderAPIProtocol::CreateRasterizationState(GPUResourceHandle handle, GPURasterizationStateCreateInfo& createInfo)
	{
		GET_STRUCT_BY_HANDLE_FROM_VECTOR(state, GPURasterizationStateCreateInfo, handle, mRasterizationStates);
		state = createInfo;
	}
	void RenderAPIProtocol::CreateMultiSampleState(GPUResourceHandle handle, GPUMultiSampleStateCreateInfo& createInfo)
	{
		GET_STRUCT_BY_HANDLE_FROM_VECTOR(state, GPUMultiSampleStateCreateInfo, handle, mMultiSampleStates);
		state = createInfo;
	}
}