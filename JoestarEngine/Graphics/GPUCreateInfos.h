#pragma once
#include "GPUTexture.h"
namespace Joestar
{
	struct GPUImageCreateInfo
	{
		ImageType type;
		ImageFormat format;
		U32 usage;
		U32 width;
		U32 height;
		U32 depth;
		U32 layer;
		U32 mipLevels;
		U32 samples;
		U32 num{1};
	};

	struct GPUImageViewCreateInfo
	{
		GPUResourceHandle imageHandle;
		ImageViewType type;
		ImageFormat format;
		U32 aspectBits;
		U32 mipLevels;
		U32 baseMipLevel;
		U32 layer;
		U32 baseLayer;
		U32 num{1};
	};

	struct GPUSwapChainCreateInfo
	{
		GPUResourceHandle imageViewHandle;
	};

	struct GPUFrameBufferCreateInfo
	{
		U32 msaaSamples{ 1 };
	};
}