#pragma once
#include "GPUTexture.h"
namespace Joestar
{
	enum class AttachmentLoadOP
	{
		LOAD = 0,
		CLEAR = 1,
		DONT_CARE = 2
	};
	enum class AttachmentStoreOP
	{
		STORE = 0,
		DONT_CARE = 1,
		NONE = 2
	};
	enum class GPUQueue
	{
		GRAPHICS = 0,
		PRESENT = 1,
		COMPUTE = 2
	};

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

	struct GPURenderPassCreateInfo
	{
		ImageFormat colorFormat;
		ImageFormat depthStencilFormat;
		AttachmentLoadOP colorLoadOp;
		AttachmentStoreOP colorStoreOp;
		AttachmentLoadOP depthLoadOp;
		AttachmentStoreOP depthStoreOp;
		AttachmentLoadOP stencilLoadOp;
		AttachmentStoreOP stencilStoreOp;
		bool clear{ false };
		
		U32 msaaSamples{ 1 };
	};

	struct GPUCommandBufferCreateInfo
	{
		GPUQueue queue;
	};
}